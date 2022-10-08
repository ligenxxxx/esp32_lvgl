#include "i2c_driver.h"

int i2c0_master_port = 0;
i2c_config_t i2c0_config = {
    .mode = I2C_MODE_MASTER,
    .sda_io_num = I2C0_MASTER_SDA_IO,
    .sda_pullup_en = GPIO_PULLUP_ENABLE,
    .scl_io_num = I2C0_MASTER_SCL_IO,
    .scl_pullup_en = GPIO_PULLUP_ENABLE,
    .master.clk_speed = I2C0_MASTER_FREQ_HZ,
};

int i2c_init(void)
{
    i2c_param_config(i2c0_master_port, &i2c0_config);
    return i2c_driver_install(i2c0_master_port, i2c0_config.mode, 0, 0, 0);
}
