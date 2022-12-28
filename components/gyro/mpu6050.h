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

#define FIFO1_SIZE 7
#define FIFO2_SIZE 7
#define ABS(a) (((a) < 0) ? (0-(a)) : (a)) 

#define RAD_TO_DEG 57.295779513082320876798154814105
typedef struct {
    int16_t Accel_X_RAW;
    int16_t Accel_Y_RAW;
    int16_t Accel_Z_RAW;
    double Ax;
    double Ay;
    double Az;

    int16_t Gyro_X_RAW;
    int16_t Gyro_Y_RAW;
    int16_t Gyro_Z_RAW;
    double Gx;
    double Gy;
    double Gz;

    float Temperature;

    double KalmanAngleX;
    double KalmanAngleY;
} MPU6050_t;

typedef struct {
    double Q_angle;
    double Q_bias;
    double R_measure;
    double angle;
    double bias;
    double P[2][2];
} Kalman_t;

void mpu6050_task();

extern double roll;
#endif