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
#include "shellTask.h"
#include "ledTask.h"
#include "uart_driver.h"

void uart_init(void)
{
    uart0_init();
}

void device_init(void)
{
    uart_init();
}

void app_main(void)
{

    device_init();
    
    printf("Hello world!\n");
    
    xTaskCreate(shellTask, "shellTask", 4096, NULL, 1, NULL);
    xTaskCreate(ledTask, "ledTask", 4096, NULL, 1, NULL);
    while(1)
    {
        vTaskDelay(1);
    }
}
