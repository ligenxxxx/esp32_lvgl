#include "../include/spi_driver.h"
#include "esp_err.h"
#include <stdint.h>

spi_device_handle_t spi2_handle;

esp_err_t spi_driver_init(void)
{
    // init spi
    spi_bus_config_t spi2_buf_cfg = {
    .miso_io_num = PIN_NUM_SPI2_MISO,
    .mosi_io_num = PIN_NUM_SPI2_MOSI,
    .sclk_io_num = PIN_NUM_SPI2_CLK,
    .quadwp_io_num = -1,
    .quadhd_io_num = -1,
    .max_transfer_sz = 320*40*2+8,
    };


    esp_err_t ret = spi_bus_initialize(HOST2, &spi2_buf_cfg, (spi_dma_chan_t)SPI2_DMA_CHANNEL);
    if(ret != ESP_OK)
        printf("\r\nspi_bus_initialize() failed");
#if(0)
    // init lcd
    spi_device_interface_config_t spi2dev_cfg = {
        .clock_speed_hz = SPI2_CLOCK_SPEED_HZ,
        .mode = SPI2_MODE,
        .spics_io_num = PIN_NUM_SPI2_CS,
        .input_delay_ns = SPI2_INPUT_DELAY_NS,
        .queue_size = SPI_TRANSACTION_POOL_SIZE,
        .pre_cb = NULL,//pre transfer callback to handle D/C line
        .post_cb = NULL,
        .flags = SPI_DEVICE_NO_DUMMY | SPI_DEVICE_HALFDUPLEX,
    };
    ret = spi_bus_add_device(HOST2, &spi2dev_cfg, &spi2_handle);
    if(ret != ESP_OK)
        printf("\r\nspi_bus_add_device() failed");
#endif
    return ret;
}