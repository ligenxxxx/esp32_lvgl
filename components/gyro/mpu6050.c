#include "driver/i2c.h"
#include "../../main/global.h"
#include "../i2c/i2c_driver.h"
#include "mpu6050.h"
#include <math.h>

mpu6050_t mpu6050;
double pitch = 0;

static esp_err_t mpu6050_init(void)
{
    uint8_t wdata[5][2] = {
        {MPU6050_REG_RA_PWR_MGMT_1, 0x01},
        {MPU6050_REG_RA_SMPLRT_DIV, 0x07},
        {MPU6050_REG_RA_CONFIG, 0x03},
        {MPU6050_REG_RA_ACCEL_CONFIG, 0x18},
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

static esp_err_t mpu6050_updata(void)
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

    wdata = MPU6050_REG_TEMP_OUT_H;
    err |= i2c_master_write_read_device(i2c1_master_port, MPU6050_I2C_ADDR, &wdata, 1, rdata, 21, I2C1_MASTER_TIMEOUT_MS / portTICK_RATE_MS);
    mpu6050.temp = (rdata[0] << 8) | rdata[1];

    return err;
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
            err |= mpu6050_updata();

            if((int16_t)mpu6050.acc_x > 0)
                pitch = atan2((double)mpu6050.acc_x, (double)mpu6050.acc_z)*180/3.14f;
            else
                pitch = 0 - atan2((double)(0-(int16_t)mpu6050.acc_x), (double)mpu6050.acc_z)*180/3.14f;

#ifdef _DEBUG_MODE
            if(err != ESP_OK)
                printf("\r\nmpu6050 error");
            else
            {
                printf("\r\npitch %.1f, acc   x:%d, y:%d, z:%d", pitch, (int16_t)mpu6050.acc_x, (int16_t)mpu6050.acc_y, (int16_t)mpu6050.acc_z);
            }
 #endif
        }
        vTaskDelay(100 / portTICK_PERIOD_MS);
    }
}