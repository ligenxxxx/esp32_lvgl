#ifndef __MPU6050_H__
#define __MPU6050_H__

#define MPU6050_I2C_ADDR                0x68

#define MPU6050_REG_RA_PWR_MGMT_1       0x6b
#define MPU6050_REG_RA_SMPLRT_DIV       0x19
#define MPU6050_REG_RA_CONFIG           0x1a
#define MPU6050_REG_RA_ACCEL_CONFIG     0x1c
#define MPU6050_REG_RA_GYRO_CONFIG      0x1B

#define MPU6050_REG_GYRO_OUT            0x43
#define MPU6050_REG_ACC_OUT             0x3b
#define MPU6050_REG_TEMP_OUT_H          0x41

typedef struct
{
    int16_t gyro_x;
    int16_t gyro_y;
    int16_t gyro_z;
    int16_t acc_x;
    int16_t acc_y;
    int16_t acc_z;
    int16_t  temp;
} mpu6050_t;

void mpu6050_task();


extern double pitch;
#endif