#include "ledTask.h"
#include <stdio.h>
#include "led_driver.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

void ledTask(void)
{
    led_init();

    while(1)
    {
        led_update(xTaskGetTickCount()*10);
        vTaskDelay(20 / portTICK_PERIOD_MS);
    }
}
