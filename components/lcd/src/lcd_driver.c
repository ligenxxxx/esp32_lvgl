#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "../include/lcd_driver.h"
#include "driver/gpio.h"
#include "esp_err.h"
#include <stdint.h>
#include <string.h>

IRAM_ATTR void spi_ready(spi_transaction_t *trans)
{
    #if(0)
    uint32_t spi_cnt = (uint32_t)trans->user;

    if(spi_cnt == 4)
    {
        spi_disp_flush_ready();
    }
    #else
    ;
    #endif
}

static void lcd_spi_pre_transfer_callback(spi_transaction_t *t)
{
    int dc = (int)t->user;
    gpio_set_level(PIN_NUM_DC, dc);
}

static void lcd_spi_driver_init()
{
    #if(0)
    spi_device_interface_config_t spi2dev_cfg = {
        .clock_speed_hz = SPI2_CLOCK_SPEED_HZ,
        .mode = SPI2_MODE,
        .spics_io_num = PIN_NUM_SPI2_CS,
        .input_delay_ns = SPI2_INPUT_DELAY_NS,
        .queue_size = SPI_TRANSACTION_POOL_SIZE,
        .pre_cb = lcd_spi_pre_transfer_callback, //pre transfer callback to handle D/C line
        .post_cb = NULL,
        .flags = SPI_DEVICE_NO_DUMMY | SPI_DEVICE_HALFDUPLEX,
    };
    #else
    spi_device_interface_config_t spi2dev_cfg = {
        .clock_speed_hz = SPI2_CLOCK_SPEED_HZ,
        .mode = SPI2_MODE,
        .spics_io_num = PIN_NUM_SPI2_CS,
        .input_delay_ns = SPI2_INPUT_DELAY_NS,
        .queue_size = SPI_TRANSACTION_POOL_SIZE,
        .cs_ena_posttrans = 1,
        .post_cb = spi_ready,
    };
    #endif

    esp_err_t ret = spi_bus_add_device(HOST2, &spi2dev_cfg, &LCD_SPI_HALDLE);
    if(ret != ESP_OK)
        printf("\r\nspi_bus_add_device() failed");
}

static void lcd_cmd(const uint8_t cmd)
{
    esp_err_t ret;
    spi_transaction_t t;
    memset(&t, 0, sizeof(t));

    t.length = 8;
    t.tx_buffer = &cmd;
    t.user = (void*)0;

    gpio_set_level(PIN_NUM_DC, 0);
    ret = spi_device_polling_transmit(LCD_SPI_HALDLE, &t);
    assert(ret == ESP_OK);
}

static void lcd_data8(uint8_t data)
{

    spi_transaction_t t;
    esp_err_t ret;
    memset(&t, 0, sizeof(t));

    t.length = 8;
    t.tx_buffer = &data;
    t.user = (void*)0;

    gpio_set_level(PIN_NUM_DC, 1);
    ret = spi_device_polling_transmit(LCD_SPI_HALDLE, &t);
    assert(ret == ESP_OK);
}

static void lcd_data(uint16_t *tbuf, uint32_t len)
{
    spi_transaction_t t;
    esp_err_t ret;
    memset(&t, 0, sizeof(t));

    t.length = len << 4;
    t.tx_buffer = (uint16_t *)tbuf;
    t.user = (void*)0;

    gpio_set_level(PIN_NUM_DC, 1);
    ret = spi_device_polling_transmit(LCD_SPI_HALDLE, &t);
    assert(ret == ESP_OK);
}

static void lcd_set_addr(uint16_t x0, uint16_t x1, uint16_t y0, uint16_t y1)
{
    lcd_cmd(0x2a);
    lcd_data8(x0 >> 8);
    lcd_data8(x0 & 0xff);
    lcd_data8(x1 >> 8);
    lcd_data8(x1 & 0xff);

    lcd_cmd(0x2b);
    lcd_data8(y0 >> 8);
    lcd_data8(y0 & 0xff);
    lcd_data8(y1 >> 8);
    lcd_data8(y1 & 0xff);

    lcd_cmd(0x2c);
}
/*
    x0/x1: range: 0 ~ LCD_HMAX-1
    y0/y1: range: 0 ~ LCD_VMAX-1

    area = [x0,x1][y0,y1]

*/
static void lcd_fill_color(uint16_t x0, uint16_t x1, uint16_t y0, uint16_t y1, uint16_t color)
{
    uint16_t i;
    const uint16_t packetSize = 2000/8; // why failed to use 4092/2 ?????
    uint16_t tbuf[packetSize];

    uint32_t tsize = (x1-x0+1)*(y1-y0+1);
    uint16_t tcnt = tsize / packetSize;
    uint16_t tsize_residual = tsize % packetSize;

    printf("\r\n%d,%d,%d,%d: %d, %d", x0, x1, y0, y1, tcnt, tsize_residual);

    //swap color
    uint8_t color_h = color >> 8;
    color = (color << 8) | color_h;

    //fill color
    for(i=0;i<packetSize;i++)
        tbuf[i] = color;

    //set area
    lcd_set_addr(x0,x1,y0,y1);

    //send integral data
    for(i=0;i<tcnt; i++)
        lcd_data(tbuf, packetSize);

    // send individual
    lcd_data(tbuf, tsize_residual);
}

#ifdef ZJY_ST7789_240_240
static void lcd_reg_init(void)
{
    lcd_cmd(0x11);
    vTaskDelay(120/portTICK_RATE_MS);
    
    lcd_cmd(ST7789_REG_MADCTRL);
    lcd_data8(0x00);

    lcd_cmd(ST7789_REG_COLMOD);
    lcd_data8(0x05);

    lcd_cmd(ST7789_REG_PORCTRL);
    lcd_data8(0x0c);
    lcd_data8(0x0c);
    lcd_data8(0x00);
    lcd_data8(0x33);
    lcd_data8(0x33);

    lcd_cmd(ST7789_REG_GCTRL);
    lcd_data8(0x35);

    lcd_cmd(ST7789_REG_VCOMS);
    lcd_data8(0x32);

    lcd_cmd(ST7789_REG_VDVVRHEN);
    lcd_data8(0x01);

    lcd_cmd(ST7789_REG_VRHS);
    lcd_data8(0x15);

    lcd_cmd(ST7789_REG_VDVS);
    lcd_data8(0x20);

    lcd_cmd(ST7789_REG_FRCTRL2);
    lcd_data8(0x0f);

    lcd_cmd(ST7789_REG_PWCTRL1);
    lcd_data8(0xa4);
    lcd_data8(0xa1);

    uint8_t i;
    const uint8_t data0[14] = {0xd0, 0x08, 0x0e, 0x09, 0x09, 0x05,
                    0x31, 0x33, 0x48, 0x17, 0x14, 0x15, 0x31, 0x34};
    lcd_cmd(ST7789_REG_PVGAMCTRL);
    for(i=0;i<14;i++)
        lcd_data8(data0[i]);

    const uint8_t data1[14] = {0xd0, 0x08, 0x0e, 0x09, 0x09, 0x15,
                    0x31, 0x33, 0x48, 0x17, 0x14, 0x15, 0x31, 0x34};
    lcd_cmd(ST7789_REG_NVGAMCTRL);
    lcd_data8(data1[i]);
    lcd_cmd(ST7789_REG_INVON);

    lcd_cmd(ST7789_REG_DISPON);
}
#endif

void lcd_driver_init(void)
{
    // init non-spi gpio
    gpio_pad_select_gpio(PIN_NUM_DC);
    gpio_pad_select_gpio(PIN_NUM_RST);
    gpio_pad_select_gpio(PIN_NUM_BLK);
    gpio_set_direction(PIN_NUM_DC, GPIO_MODE_OUTPUT);
    gpio_set_direction(PIN_NUM_RST, GPIO_MODE_OUTPUT);
    gpio_set_direction(PIN_NUM_BLK, GPIO_MODE_OUTPUT);

    //init spi
    spi_driver_init();
    lcd_spi_driver_init();

    //reset
    gpio_set_level(PIN_NUM_RST, 0);
    vTaskDelay(100 / portTICK_RATE_MS);
    gpio_set_level(PIN_NUM_RST, 1);
    vTaskDelay(100 / portTICK_RATE_MS);
    //backlight
    gpio_set_level(PIN_NUM_BLK, 1);
    vTaskDelay(100 / portTICK_RATE_MS);

    //detect LCD type
    #if(0)
    uint32_t lcd_id = lcd_get_id();
    printf("\r\nLCD ID:0X%08X", lcd_id);
    #endif

    lcd_reg_init();

    // fill color
    
    //draw all
    lcd_fill_color(0,LCD_HMAX-1,0,LCD_VMAX-1,COLOR_WHITE);
    //draw top
    lcd_fill_color(0,LCD_HMAX-1,0,7,COLOR_BLUE);
    //draw bottom
    lcd_fill_color(0,LCD_HMAX-1,LCD_VMAX-8,LCD_VMAX-1,COLOR_GREEN);
    //draw right
    lcd_fill_color(LCD_HMAX-1-7,LCD_HMAX-1,0,LCD_VMAX-1,COLOR_YELLOW);
    //draw left
    lcd_fill_color(0,7,0,LCD_VMAX-1,COLOR_RED);
}