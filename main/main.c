/* Hello World Example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/
#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "../components/led/led_driver.h"
#include "../components/uart/uart_driver.h"
#include "../components/i2c/i2c_driver.h"
#include "../components/timer/timer.h"
#include "../components/lcd/lcd_driver.h"
#include "../components/joystick/joystick_driver.h"
#include "../components/shell/shell.h"

void uart_init(void)
{
    uart0_init();
}

void device_init(void)
{
    uart_init();
    i2c_init();
    lcd_driver_init();

    printf("\r\ndevice init done");
}

void app_main(void)
{
    device_init();
    
    xTaskCreate(ledTask, "ledTask", 4096, NULL, 1, NULL);
    xTaskCreate(shellTask, "shellTask", 4096, NULL, 1, NULL);
    xTaskCreate(lcdTask, "lvglTask", 4096, NULL, 1, NULL);

    while(1)
    {
        vTaskDelay(1);
    }
}
