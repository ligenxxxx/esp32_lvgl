#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "../include/lcd_driver.h"
#include "driver/gpio.h"
#include "esp_err.h"
#include <stdint.h>
#include <string.h>

static void lcd_spi_pre_transfer_callback(spi_transaction_t *t)
{
    int dc = (int)t->user;
    gpio_set_level(PIN_NUM_DC, dc);
}

static void lcd_spi_driver_init()
{
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

    esp_err_t ret = spi_bus_add_device(HOST2, &spi2dev_cfg, &LCD_SPI_HALDLE);
    if(ret != ESP_OK)
        printf("\r\nspi_bus_add_device() failed");
}

static void lcd_cmd(const uint8_t cmd)
{
    spi_transaction_t t;
    esp_err_t ret;
    memset(&t, 0, sizeof(t));

    t.length = 8;
    t.tx_buffer = &cmd;
    t.user = (void*)0;

    ret = spi_device_transmit(LCD_SPI_HALDLE, &t);
    assert(ret == ESP_OK);
}
static void lcd_data(const uint8_t *data, uint16_t len)
{
    spi_transaction_t t;
    esp_err_t ret;
    memset(&t, 0, sizeof(t));

    t.length = len << 3;
    t.tx_buffer = data;
    t.user = (void*)1;

    ret = spi_device_transmit(LCD_SPI_HALDLE, &t);
    assert(ret == ESP_OK);
}

static void lcd_data8(uint8_t data)
{

    spi_transaction_t t;
    esp_err_t ret;
    memset(&t, 0, sizeof(t));

    t.length = 8;
    t.tx_buffer = &data;
    t.user = (void*)1;

    ret = spi_device_transmit(LCD_SPI_HALDLE, &t);
    assert(ret == ESP_OK);
}

#if(0)
static uint32_t lcd_get_id(void)
{
    esp_err_t ret;
    uint8_t data;


    spi_transaction_t t;
    memset(&t, 0, sizeof(t));
    t.length = 8*3;
    t.flags = SPI_TRANS_USE_RXDATA;
    t.user = (void*)1;

    ret = spi_device_transmit(LCD_SPI_HALDLE, &t);
    assert(ret == ESP_OK);

    return *(uint32_t*)t.rx_data;
}
#endif

static void lcd_addr_set(uint16_t x0, uint16_t x1, uint16_t y0, uint16_t y1)
{
    lcd_cmd(0x2a);
    lcd_data8(x0>>8);
    lcd_data8(x0&0xff);
    lcd_data8(x1>>8);
    lcd_data8(x1&0xff);

    lcd_cmd(0x2b);
    lcd_data8((y0+34)>>8);
    lcd_data8((y0+34)&0xff);
    
    lcd_cmd(0x2c);
}

static void lcd_fill(uint16_t x0, uint16_t x1, uint16_t y0, uint16_t y1, uint16_t color)
{
    uint16_t x,y;
    lcd_addr_set(x0,x1,y0,y1);
    for(y=y0; y<y1; y++)
    {
        for(x=x0; x<x1; x++)
        {
            lcd_data8(color>>8);
            lcd_data8(color&0xff);
        }
    }
}

static void lcd_reg_init(void)
{
    lcd_cmd(0x11);
    
    lcd_cmd(0x36);
    lcd_data8(0x70);

    lcd_cmd(0x3a);
    lcd_data8(0x05);

    lcd_cmd(0xb2);
    lcd_data8(0x0c);
    lcd_data8(0x0c);
    lcd_data8(0x00);
    lcd_data8(0x33);
    lcd_data8(0x33);

    lcd_cmd(0xb7);
    lcd_data8(0x35);

    lcd_cmd(0xbb);
    lcd_data8(0x35);

    lcd_cmd(0xc0);
    lcd_data8(0x2c);

    lcd_cmd(0xc2);
    lcd_data8(0x01);

    lcd_cmd(0xc3);
    lcd_data8(0x13);

    lcd_cmd(0xc4);
    lcd_data8(0x20);

    lcd_cmd(0xc6);
    lcd_data8(0x0f);

    lcd_cmd(0xd0);
    lcd_data8(0xa4);
    lcd_data8(0xa1);

    lcd_cmd(0xd6);
    lcd_data8(0xa1);

    uint8_t data0[14] = {0xf0, 0x00, 0x04, 0x04, 0x04, 0x05,
                    0x29, 0x33, 0x3e, 0x38, 0x12, 0x12, 0x28, 0x30};
    lcd_cmd(0xe0);
    lcd_data(data0, sizeof(data0));

    uint8_t data1[14] = {0xf0, 0x07, 0x0a, 0x0d, 0x0b, 0x07,
                    0x28, 0x33, 0x3e, 0x36, 0x14, 0x14, 0x29, 0x32};
    lcd_cmd(0xe1);
    lcd_data(data0, sizeof(data1));

    lcd_cmd(0x21);

    lcd_cmd(0x11);
    vTaskDelay(100 / portTICK_RATE_MS);
    lcd_cmd(0x29);
}

void lcd_driver_init(void)
{
    //init spi
    lcd_spi_driver_init();

    // init non-spi gpio
    gpio_set_direction(PIN_NUM_DC, GPIO_MODE_OUTPUT);
    gpio_set_direction(PIN_NUM_RST, GPIO_MODE_OUTPUT);
    gpio_set_direction(PIN_NUM_BLK, GPIO_MODE_OUTPUT);

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

    // fill white
    lcd_fill(0,100,0,100,0xffff);
}