#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "lcd_driver.h"
#include "driver/gpio.h"
#include "esp_err.h"
#include <stdint.h>
#include <string.h>

#if(1)
extern void spi_disp_flush_ready(void);
IRAM_ATTR void spi_ready(spi_transaction_t *trans)
{
    uint32_t is_tail = (uint32_t)trans->user;

    if(is_tail)
    {
        spi_disp_flush_ready();
    }
}
#endif

static void lcd_spi_driver_init()
{
    spi_device_interface_config_t spi2dev_cfg = {
        .clock_speed_hz = SPI2_CLOCK_SPEED_HZ,
        .mode = SPI2_MODE,
        .spics_io_num = PIN_NUM_SPI2_CS,
        .input_delay_ns = SPI2_INPUT_DELAY_NS,
        .queue_size = SPI2_QUEUE_SIZE,
        .cs_ena_posttrans = 1,
        .pre_cb = NULL,
        .post_cb = spi_ready,
    };

    esp_err_t ret = spi_bus_add_device(HOST2, &spi2dev_cfg, &LCD_SPI_HANDLE);
    ESP_ERROR_CHECK(ret);
}

IRAM_ATTR static void lcd_cmd(const uint8_t cmd)
{
    esp_err_t ret;
    spi_transaction_t t;
    memset(&t, 0, sizeof(t));

    t.length = 8;
    t.tx_buffer = &cmd;
    //t.user = (void*)0;
    LCD_CMD_MODE;
    ret = spi_device_polling_transmit(LCD_SPI_HANDLE, &t);
    assert(ret == ESP_OK);

    LCD_DAT_MODE;
}

IRAM_ATTR static void lcd_data8(uint8_t data)
{

    spi_transaction_t t;
    esp_err_t ret;
    memset(&t, 0, sizeof(t));

    t.length = 8;
    t.tx_buffer = &data;
    //t.user = (void*)1;
    ret = spi_device_polling_transmit(LCD_SPI_HANDLE, &t);
    assert(ret == ESP_OK);
}

IRAM_ATTR static void lcd_set_addr(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2)
{
    lcd_cmd(0x2a);
    lcd_data8(x1 >> 8);
    lcd_data8(x1 & 0xff);
    lcd_data8(x2 >> 8);
    lcd_data8(x2 & 0xff);

    lcd_cmd(0x2b);
    lcd_data8(y1 >> 8);
    lcd_data8(y1 & 0xff);
    lcd_data8(y2 >> 8);
    lcd_data8(y2 & 0xff);

    lcd_cmd(0x2c);
}
IRAM_ATTR void lcd_data_x(uint16_t *dat, uint32_t len, uint32_t is_tail)
{
    #if(0)
    spi_transaction_t t;
    memset(&t, 0, sizeof(t));                             // Zero out the transaction
    t.length = len;                                       // Command is 8 bits
    t.tx_buffer = dat;                                    // The data is the cmd itself
    t.user = (void *)is_tail;                             // D/C needs to be set to 0

    esp_err_t ret = spi_device_polling_transmit(LCD_SPI_HANDLE, &t); // Transmit!
    ESP_ERROR_CHECK(ret);
    #else
    static spi_transaction_t t[SPI2_QUEUE_SIZE];
    static uint8_t i;
    
    memset(&t[i], 0, sizeof(spi_transaction_t));          // Zero out the transaction
    t[i].length = len;                                       // Command is 8 bits
    t[i].tx_buffer = dat;                                    // The data is the cmd itself
    t[i].user = (void *)is_tail;                             // D/C needs to be set to 0

    esp_err_t ret = spi_device_queue_trans(LCD_SPI_HANDLE, &t[i], portMAX_DELAY); // Transmit!
    ESP_ERROR_CHECK(ret);
    i++;
    if(i==SPI2_QUEUE_SIZE)
        i = 0;
    #endif
}

/*
    x0/x1: range: 0 ~ LCD_HMAX-1
    y0/y1: range: 0 ~ LCD_VMAX-1

    area = [x0,x1][y0,y1]

*/
IRAM_ATTR void lcd_show(uint16_t x1, uint16_t y1,uint16_t x2, uint16_t y2, uint16_t *color_p)
{
    uint32_t tsize = (x2 + 1 - x1) * (y2 + 1 - y1) * 2; //transmit size in u8
    uint32_t psize_u8 = LCD_HMAX * 2 * 10; //package size in u8 (10 lines)
    uint32_t psize_u16 = psize_u8 >> 1; //package size in u16
    uint32_t fsize_u16 = (tsize % psize_u8) >> 1; //flush size in u16
    uint32_t pcnt = tsize / psize_u8;  //package cnt
    uint32_t i=0;

    lcd_set_addr(x1, y1, x2, y2);

    if (tsize == 0)
    {
        return;
    }

    if(pcnt)
    {
        for (i = 0; i < pcnt; i++)
        {
            if ((i + 1) == pcnt && fsize_u16 == 0)
            {
                lcd_data_x(&color_p[i * psize_u16], (psize_u16 << 4), 1);
            }
            else
            {
                lcd_data_x(&color_p[i * psize_u16], (psize_u16 << 4), 0);
            }
        }
    }
    if (fsize_u16)
    {
        lcd_data_x(&color_p[i * psize_u16], (fsize_u16 << 4), 1);
    }
}

#ifdef ZJY_ST7789_240_240
static void lcd_init(void)
{
    // init non-spi gpio
    gpio_pad_select_gpio(PIN_NUM_DC);
    gpio_pad_select_gpio(PIN_NUM_RST);
    gpio_pad_select_gpio(PIN_NUM_BLK);
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
    for(i=0;i<14;i++)
        lcd_data8(data1[i]);

    lcd_cmd(ST7789_REG_INVON);
    lcd_cmd(ST7789_REG_SLPOUT);
    lcd_cmd(ST7789_REG_DISPON);
}
#endif

void lcd_driver_init(void)
{

    //init spi
    spi_driver_init();
    lcd_spi_driver_init();

    lcd_init();
}