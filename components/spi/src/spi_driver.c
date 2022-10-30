#include "../include/spi_driver.h"
#include "esp_err.h"
#include <stdint.h>

spi_device_handle_t spi2_handle;

esp_err_t spi_driver_init(void)
{
    // init spi
    spi_bus_config_t spi2_buf_cfg = {
    .mosi_io_num = PIN_NUM_SPI2_MOSI,
    .miso_io_num = PIN_NUM_SPI2_MISO,
    .sclk_io_num = PIN_NUM_SPI2_CLK,
    .quadwp_io_num = -1,
    .quadhd_io_num = -1,
    .max_transfer_sz = 4094,
    };


    esp_err_t ret = spi_bus_initialize(HOST2, &spi2_buf_cfg, SPI_DMA_CH_AUTO);
    if(ret != ESP_OK)
        printf("\r\nspi_bus_initialize() failed");

    return ret;
}