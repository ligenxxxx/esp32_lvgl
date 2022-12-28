#include "driver/i2c.h"
#include "../../main/global.h"
#include "../i2c/i2c_driver.h"
#include "mpu6050.h"
#include <math.h>

const uint16_t SAMPLE_PERIOD = 20; //ms
const uint16_t SAMPLE_RATE = 1000/20; //ms
const double dt = (double)SAMPLE_PERIOD / 1000;

MPU6050_t mpu6050;

Kalman_t KalmanX = {
    .Q_angle = 0.001f,
    .Q_bias = 0x003f,
    .R_measure = 0x03f
};
Kalman_t KalmanY = {
    .Q_angle = 0.001f,
    .Q_bias = 0.003f,
    .R_measure = 0.03f
};
double roll = 0;
static double fifo1[FIFO1_SIZE] = {0};
//static double fifo2[FIFO1_SIZE] = {0};

double Kalman_getAngle(Kalman_t* Kalman, double newAngle, double newRate, double dt);

static esp_err_t mpu6050_init(void)
{
    uint8_t wdata[5][2] = {
        {MPU6050_REG_RA_PWR_MGMT_1, 0x01},
        {MPU6050_REG_RA_SMPLRT_DIV, 1000/SAMPLE_RATE - 1}, // 1000 decided by MPU6050_REG_RA_CONFIG(DLPF_CFG)
        {MPU6050_REG_RA_CONFIG, 0x04},
        {MPU6050_REG_RA_ACCEL_CONFIG, 0x00},//0x18
        {MPU6050_REG_RA_GYRO_CONFIG, 0x18}
    };
    int i;
    esp_err_t err = ESP_OK;

    for(i=0;i<5;i++)
    {
        err |= i2c_master_write_to_device(i2c1_master_port, MPU6050_I2C_ADDR, wdata[i], 2, I2C1_MASTER_TIMEOUT_MS / portTICK_RATE_MS);
    }

    return err;
}

static esp_err_t mpu6050_read_data(void)
{
    esp_err_t err;
    uint8_t wdata;
    uint8_t rdata[6];
    double roll_raw, pitch_raw;
    double roll_sqrt;

    //read data
    wdata = MPU6050_REG_GYRO_OUT;
    err = i2c_master_write_read_device(i2c1_master_port, MPU6050_I2C_ADDR, &wdata, 1, rdata, 6, I2C1_MASTER_TIMEOUT_MS / portTICK_RATE_MS);
    mpu6050.Gyro_X_RAW = (int16_t)((rdata[0] << 8) | rdata[1]);
    mpu6050.Gyro_Y_RAW = (int16_t)((rdata[2] << 8) | rdata[3]);
    mpu6050.Gyro_Z_RAW = (int16_t)((rdata[4] << 8) | rdata[5]);

    wdata = MPU6050_REG_ACC_OUT;
    err |= i2c_master_write_read_device(i2c1_master_port, MPU6050_I2C_ADDR, &wdata, 1, rdata, 6, I2C1_MASTER_TIMEOUT_MS / portTICK_RATE_MS);
    mpu6050.Accel_X_RAW = (int16_t)((rdata[0] << 8) | rdata[1]);
    mpu6050.Accel_Y_RAW = (int16_t)((rdata[2] << 8) | rdata[3]);
    mpu6050.Accel_Z_RAW = (int16_t)((rdata[4] << 8) | rdata[5]);

    mpu6050.Ax = mpu6050.Accel_X_RAW / 16384.0;
    mpu6050.Ay = mpu6050.Accel_Y_RAW / 16384.0;
    mpu6050.Az = mpu6050.Accel_Z_RAW / 16384.0;
    mpu6050.Gx = mpu6050.Gyro_X_RAW / 131.0;
    mpu6050.Gy = mpu6050.Gyro_Y_RAW / 131.0;
    mpu6050.Gz = mpu6050.Gyro_Z_RAW / 131.0;

    roll_sqrt = sqrt( mpu6050.Accel_X_RAW * mpu6050.Accel_X_RAW + mpu6050.Accel_Z_RAW * mpu6050.Accel_Z_RAW);
    if (roll_sqrt != 0.0)
    {
        roll_raw = atan(mpu6050.Accel_Y_RAW / roll_sqrt) * RAD_TO_DEG;
    }
    else {
        roll_raw = 0.0;
    }

    pitch_raw = atan2(-mpu6050.Accel_X_RAW, mpu6050.Accel_Z_RAW) * RAD_TO_DEG;
    if ((pitch_raw < -90 && mpu6050.KalmanAngleY > 90) || (pitch_raw > 90 && mpu6050.KalmanAngleY < -90))
    {
        KalmanY.angle = pitch_raw;
        mpu6050.KalmanAngleY = pitch_raw;
    }
    else
    {
        mpu6050.KalmanAngleY = Kalman_getAngle(&KalmanY, pitch_raw, mpu6050.Gy, dt);
    }
    
    if (fabs(mpu6050.KalmanAngleY) > 90)
        mpu6050.Gx = -mpu6050.Gx;
    mpu6050.KalmanAngleX = Kalman_getAngle(&KalmanX, roll_raw, mpu6050.Gy, dt);

    return err;
}

double Kalman_getAngle(Kalman_t* Kalman, double newAngle, double newRate, double dt) {
    double rate = newRate - Kalman->bias;
    Kalman->angle += dt * rate;

    Kalman->P[0][0] += dt * (dt * Kalman->P[1][1] - Kalman->P[0][1] - Kalman->P[1][0] + Kalman->Q_angle);
    Kalman->P[0][1] -= dt * Kalman->P[1][1];
    Kalman->P[1][0] -= dt * Kalman->P[1][1];
    Kalman->P[1][1] += Kalman->Q_bias * dt;

    double S = Kalman->P[0][0] + Kalman->R_measure;
    double K[2];
    K[0] = Kalman->P[0][0] / S;
    K[1] = Kalman->P[1][0] / S;

    double y = newAngle - Kalman->angle;
    Kalman->angle += K[0] * y;
    Kalman->bias += K[1] * y;

    double P00_temp = Kalman->P[0][0];
    double P01_temp = Kalman->P[0][1];

    Kalman->P[0][0] -= K[0] * P00_temp;
    Kalman->P[0][1] -= K[0] * P01_temp;
    Kalman->P[1][0] -= K[1] * P00_temp;
    Kalman->P[1][1] -= K[1] * P01_temp;

    return Kalman->angle;
};

static void fifo_write(double *fifo, double angle, int fifo_size)
{
    int i;

    for(i=fifo_size-1; i>=0; i--)
    {
        if(i==0)
            fifo[i] = angle;
        else
            fifo[i] = fifo[i-1];
    }
}

static double lp_filter(double *fifo, int fifo_size)
{
    double angle = 0;
    int i;

    for(i=0; i<fifo_size; i++)
        angle += fifo[i];

    angle /= fifo_size;

    //printf("lp_filter:%.2f\n", angle);
    return angle;
}
#if(0)
static double dynamic_filter(void)
{
    double avg = 0;
    double mess = 0;
    static double angle = 0;
    int i;

    for(i=0; i<FIFO1_SIZE; i++)
        avg += fifo1[i];
    avg /= FIFO1_SIZE;

    for(i=0; i<FIFO1_SIZE; i++)
        mess += ABS(fifo1[i] - avg);
    
    if(mess <= 5)
    {
        if(angle - avg > 2)
        {
            if((mpu6050.gyro_y / 16.384 / 50) < -0.5)
                angle = avg;
            else
                angle += (mpu6050.gyro_y / 16.384 / 50);
        }
        else
        {
            angle = avg;
        }
    }
    else
    {
        angle += (mpu6050.gyro_y / 16.384 / 50);
    }

    //printf("dynamic_filter:%.2f %.2f %.2f\n", angle, mess, mpu6050.gyro_y / 16.384 / 50);
    return angle;
}

#endif

void mpu6050_task()
{
    esp_err_t err;

    err = mpu6050_init();
#ifdef _DEBUG_MODE
    if(err)
        printf("\r\nmpu6050 init failed");
    else
        printf("\r\nmpu6050 init done");
#endif

    while (1)
    {
        if(err == ESP_OK)
        {
            err |= mpu6050_read_data();
            fifo_write(fifo1, mpu6050.KalmanAngleY, FIFO1_SIZE);
            roll = lp_filter(fifo1, FIFO1_SIZE);
            printf("roll:%.1f\n", roll);
        }
        vTaskDelay(SAMPLE_PERIOD / portTICK_PERIOD_MS);
    }
}