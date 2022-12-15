#include "driver/i2c.h"
#include "../../main/global.h"
#include "../i2c/i2c_driver.h"
#include "mpu6050.h"
#include <math.h>

const uint16_t SAMPLE_PERIOD = 20; //ms
const uint16_t SAMPLE_RATE = 1000/20; //ms
mpu6050_t mpu6050;
double pitch = 0;

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

#if(0)
    // deg/gyro_data = 2000/32768
    // => deg = gyro_data / (32768/2000)
    // => deg = ro_data / 16.384
    mpu6050.gyro_x /= 16.384f;
    mpu6050.gyro_y /= 16.384f;
    mpu6050.gyro_z /= 16.384f;
#endif
    wdata = MPU6050_REG_ACC_OUT;
    err |= i2c_master_write_read_device(i2c1_master_port, MPU6050_I2C_ADDR, &wdata, 1, rdata, 6, I2C1_MASTER_TIMEOUT_MS / portTICK_RATE_MS);
    mpu6050.acc_x = (rdata[0] << 8) | rdata[1];
    mpu6050.acc_y = (rdata[2] << 8) | rdata[3];
    mpu6050.acc_z = (rdata[4] << 8) | rdata[5];
#if(0)
    // a/acc_data = 2/32768
    // => a = acc_data / (32768/2)
    // => a = acc_data / 16384.0
    mpu6050.acc_x /= 16384.0f;
    mpu6050.acc_y /= 16384.0f;
    mpu6050.acc_z /= 16384.0f;
#endif
    #if(0)
    wdata = MPU6050_REG_TEMP_OUT_H;
    err |= i2c_master_write_read_device(i2c1_master_port, MPU6050_I2C_ADDR, &wdata, 1, rdata, 21, I2C1_MASTER_TIMEOUT_MS / portTICK_RATE_MS);
    mpu6050.temp = (rdata[0] << 8) | rdata[1];
    #endif

    printf("\r\n%d %d %d %d %d %d",mpu6050.gyro_x,mpu6050.gyro_y,mpu6050.gyro_z,
                                        mpu6050.acc_x,mpu6050.acc_y,mpu6050.acc_z);
    return err;
}

static void complementary_filter(void)
{
    float angle = 0;
    float e_gyro = 0;
    const float Ki = 0;
    static double pitch_r = 0;
    static float e_gyro_r = 1;
    #if(1)
            if((int16_t)mpu6050.acc_x > 0)
                angle = atan2((double)mpu6050.acc_x, (double)mpu6050.acc_z)*180/3.14f;
            else
                angle = 0 - atan2((double)(0-(int16_t)mpu6050.acc_x), (double)mpu6050.acc_z)*180/3.14f;
    #else
        angle = atan2((double)mpu6050.acc_x, (double)mpu6050.acc_z)*180/3.14f;
    #endif
    #if(0)
    if((int16_t)mpu6050.gyro_y > 0)
        e_gyro = (int16_t)mpu6050.gyro_y * 20 / 1000.f;
    else
        e_gyro = 0 - (int16_t)mpu6050.gyro_y * 20 / 1000.f;
    #else
        e_gyro = (int16_t)mpu6050.gyro_y * 20 / 1000.f;
    #endif
    pitch_r = Ki * (e_gyro - e_gyro_r + pitch_r) + (1 - Ki) * angle;
    printf("\r\nangle%.1f, e_gyro%.1f, pitch_r:%.1f", angle, e_gyro-e_gyro_r, pitch_r);
    pitch = pitch_r;
#if(0)
    if((int16_t)mpu6050.acc_x < 0)
        pitch = 0 - pitch_r;
    else
        pitch = pitch_r;
#endif
    e_gyro_r = e_gyro;
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
        }
        vTaskDelay(SAMPLE_PERIOD / portTICK_PERIOD_MS);
    }
}