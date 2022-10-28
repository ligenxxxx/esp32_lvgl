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

    t.length = (len << 3);
    t.tx_buffer = data;
    t.user = (void*)1;

    ret = spi_device_transmit(LCD_SPI_HALDLE, &t);
    assert(ret == ESP_OK);
}

static uint32_t lcd_get_id(void)
{
    esp_err_t ret;

    lcd_cmd(0x04);

    spi_transaction_t t;
    memset(&t, 0, sizeof(t));
    t.length = 8*3;
    t.flags = SPI_TRANS_USE_RXDATA;
    t.user = (void*)1;

    ret = spi_device_transmit(LCD_SPI_HALDLE, &t);
    assert(ret == ESP_OK);

    return *(uint32_t*)t.rx_data;
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

    //detect LCD type
    uint32_t lcd_id = lcd_get_id();
    printf("\r\nLCD ID:0X%08X", lcd_id);
}