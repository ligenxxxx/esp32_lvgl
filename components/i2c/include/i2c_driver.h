#ifndef __LED_DRIVER_H__
#define __LED_DRIVER_H__

#include "driver/i2c.h"
//#include "driver/gpio.h"

#define I2C0_MASTER_SCL_IO   22
#define I2C0_MASTER_SDA_IO   23
#define I2C0_MASTER_FREQ_HZ  100000


int i2c_init(void);

#endif