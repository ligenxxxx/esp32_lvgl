#ifndef __LCD_DRIVER_H__
#define __LCD_DRIVER_H__
#include "driver/spi_common.h"
#include "driver/spi_master.h"
#include "../../spi/include/spi_driver.h"

#define PIN_NUM_DC      2
#define PIN_NUM_RST     4
#define PIN_NUM_BLK     5

#define LCD_SPI_HALDLE  spi2_handle

#define LCD_HMAX        320
#define LCD_VMAX        172

#define COLOR_WHITE     0xffff
#define COLOR_BLACK     0x0000
#define COLOR_BLUE      0x001f
#define COLOR_GREEN     0x07e0
#define COLOR_RED       0xf800
#define COLOR_YELLOW    0xffe0

void lcd_driver_init(void);
#endif