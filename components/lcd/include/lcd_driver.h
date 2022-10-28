#ifndef __LCD_DRIVER_H__
#define __LCD_DRIVER_H__
#include "driver/spi_common.h"
#include "driver/spi_master.h"
#include "../../spi/include/spi_driver.h"

#define PIN_NUM_DC      2
#define PIN_NUM_RST     4
#define PIN_NUM_BLK     5

#define LCD_SPI_HALDLE  spi2_handle

void lcd_driver_init(void);
#endif