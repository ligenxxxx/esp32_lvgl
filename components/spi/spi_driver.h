#ifndef __SPI_DRIVER_H__
#define __SPI_DRIVER_H__
#include "driver/spi_common.h"
#include "driver/spi_master.h"

#define HOST2               SPI2_HOST
#define PIN_NUM_SPI2_MISO   -1
#define PIN_NUM_SPI2_MOSI   13
#define PIN_NUM_SPI2_CLK    14
#define PIN_NUM_SPI2_CS     15

#define SPI2_DMA_CHANNEL    1
#define SPI2_CLOCK_SPEED_HZ 40000000
#define SPI2_MODE           0
#define SPI2_INPUT_DELAY_NS 0
#define SPI2_QUEUE_SIZE     80

esp_err_t spi_driver_init(void);

extern spi_device_handle_t spi2_handle;
#endif