#include "driver/i2c.h"
#include "../../main/global.h"
#include "../i2c/i2c_driver.h"
#include "mpu6050.h"
#include <math.h>

const uint16_t SAMPLE_PERIOD = 20; //ms
const uint16_t SAMPLE_RATE = 1000/20; //ms
mpu6050_t mpu6050;
double pitch = 0;
static double fifo1[FIFO1_SIZE] = {0};
static double fifo2[FIFO1_SIZE] = {0};

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

    wdata = MPU6050_REG_GYRO_OUT;
    err = i2c_master_write_read_device(i2c1_master_port, MPU6050_I2C_ADDR, &wdata, 1, rdata, 6, I2C1_MASTER_TIMEOUT_MS / portTICK_RATE_MS);
    mpu6050.gyro_x = (rdata[0] << 8) | rdata[1];
    mpu6050.gyro_y = (rdata[2] << 8) | rdata[3];
    mpu6050.gyro_z = (rdata[4] << 8) | rdata[5];

    wdata = MPU6050_REG_ACC_OUT;
    err |= i2c_master_write_read_device(i2c1_master_port, MPU6050_I2C_ADDR, &wdata, 1, rdata, 6, I2C1_MASTER_TIMEOUT_MS / portTICK_RATE_MS);
    mpu6050.acc_x = (rdata[0] << 8) | rdata[1];
    mpu6050.acc_y = (rdata[2] << 8) | rdata[3];
    mpu6050.acc_z = (rdata[4] << 8) | rdata[5];

#if(0)
    printf("\r\n%d %d %d %d %d %d",mpu6050.gyro_x,mpu6050.gyro_y,mpu6050.gyro_z,
                                        mpu6050.acc_x,mpu6050.acc_y,mpu6050.acc_z);
#endif
    return err;
}

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
            fifo_write(fifo1, (atan2((double)mpu6050.acc_z, (double)mpu6050.acc_x) * 57.2957795), FIFO1_SIZE);
            fifo_write(fifo2, dynamic_filter(), FIFO2_SIZE);
            pitch = lp_filter(fifo2, FIFO2_SIZE);
        }
        vTaskDelay(SAMPLE_PERIOD / portTICK_PERIOD_MS);
    }
}