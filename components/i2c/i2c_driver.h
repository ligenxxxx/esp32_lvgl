#ifndef __I2C_DRIVER_H__
#define __I2C_DRIVER_H__

#include "driver/i2c.h"

#define I2C0_MASTER_SCL_IO   22
#define I2C0_MASTER_SDA_IO   23
#define I2C0_MASTER_FREQ_HZ  100000
#define I2C0_MASTER_TIMEOUT_MS 1000

#define I2C1_MASTER_SCL_IO   19
#define I2C1_MASTER_SDA_IO   18
#define I2C1_MASTER_FREQ_HZ  400000
#define I2C1_MASTER_TIMEOUT_MS 1000

esp_err_t i2c_init(void);

extern int i2c0_master_port;
extern int i2c1_master_port;
#endif