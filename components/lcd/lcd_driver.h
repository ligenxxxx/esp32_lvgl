#ifndef __LCD_DRIVER_H__
#define __LCD_DRIVER_H__
#include "driver/spi_common.h"
#include "driver/spi_master.h"
#include "../spi/spi_driver.h"

#define ZJY_ST7789_240_240

#define LCD_SPI_HANDLE  (spi2_handle)

#ifdef ZJY_ST7789_240_240
#define PIN_NUM_DC      2
#define PIN_NUM_RST     23
#define PIN_NUM_BLK     5
#define LCD_HMAX        240
#define LCD_VMAX        240
#endif

#define LCD_DAT_MODE    gpio_set_level(PIN_NUM_DC, 1)
#define LCD_CMD_MODE    gpio_set_level(PIN_NUM_DC, 0)

#define ST7789_REG_SLPIN        0x10
#define ST7789_REG_SLPOUT       0x11
#define ST7789_REG_PTLON        0x12
#define ST7789_REG_NORON        0x13
#define ST7789_REG_INVOFF       0x20
#define ST7789_REG_INVON        0x21
#define ST7789_REG_GAMSET       0x26
#define ST7789_REG_DISPOFF      0x28
#define ST7789_REG_DISPON       0x29
#define ST7789_REG_CASET        0x2A
#define ST7789_REG_RASET        0x2B
#define ST7789_REG_RAMWR        0x2C
#define ST7789_REG_RAMRD        0x2E
#define ST7789_REG_PTLAR        0x30
#define ST7789_REG_VSRDEF       0x33
#define ST7789_REG_TEOFF        0x34
#define ST7789_REG_TEON         0x35
#define ST7789_REG_MADCTRL      0x36
#define ST7789_REG_VSCSAD       0x37
#define ST7789_REG_IDMOFF       0x38
#define ST7789_REG_IDMON        0x39
#define ST7789_REG_COLMOD       0x3A
#define ST7789_REG_WRMEMC       0x3C
#define ST7789_REG_RDMEMC       0x3E
#define ST7789_REG_STE          0x44
#define ST7789_REG_GSCAN        0x45
#define ST7789_REG_WRDISBV      0x51
#define ST7789_REG_RDDISBV      0x52
#define ST7789_REG_WRCTRLD      0x53
#define ST7789_REG_RDCTRLD      0x54
#define ST7789_REG_WRCACE       0x55
#define ST7789_REG_RDCABC       0x56

#define ST7789_REG_PORCTRL      0xB2
#define ST7789_REG_GCTRL        0xB7
#define ST7789_REG_VCOMS        0xBB
#define ST7789_REG_LCMCTRL      0xC0
#define ST7789_REG_VDVVRHEN     0xC2
#define ST7789_REG_VRHS         0xC3
#define ST7789_REG_VDVS         0xC4
#define ST7789_REG_FRCTRL2      0xC6
#define ST7789_REG_PWCTRL1      0xD0
#define ST7789_REG_PVGAMCTRL    0xE0
#define ST7789_REG_NVGAMCTRL    0xE1

#define COLOR_WHITE     0xffff
#define COLOR_BLACK     0x0000
#define COLOR_BLUE      0x001f
#define COLOR_GREEN     0x07e0
#define COLOR_RED       0xf800
#define COLOR_YELLOW    0xffe0

void lcd_driver_init(void);
#if(0)
void lcdTask();
#endif
void lcd_show(uint16_t x1, uint16_t y1,uint16_t x2, uint16_t y2, uint16_t *color_p);
#endif