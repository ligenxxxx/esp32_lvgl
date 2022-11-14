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
#include "esp_log.h"
#include "../components/led/led_driver.h"
#include "../components/uart/uart_driver.h"
#include "../components/i2c/i2c_driver.h"
#include "../components/timer/timer.h"
#include "../components/lcd/lcd_driver.h"
#include "../components/joystick/joystick_driver.h"
#include "../components/shell/shell.h"
#include "../components/lvgl/lvgl.h"
#include "../components/lvgl/porting/lv_port_disp.h"
#include "../components/lvgl/demos/lv_demos.h"

#if(1)

static esp_timer_handle_t lvgl_timer_handle = NULL;
static IRAM_ATTR void lv_timer_cb(void *arg)
{
   lv_tick_inc(1);
}

static esp_timer_create_args_t lvgl_timer = {
    .callback = &lv_timer_cb,
    .arg = NULL,
    .name ="lvgl_timer",
    .dispatch_method = ESP_TIMER_TASK
};

esp_err_t timer_init(void)
{
   esp_err_t err = esp_timer_create(&lvgl_timer, &lvgl_timer_handle);
   err = esp_timer_start_periodic(lvgl_timer_handle, 1000); // 1ms
   return err;
}

IRAM_ATTR void lvgl_task(void *arg)
{
    #if(0)
    lv_obj_t *btn1 = lv_btn_create(lv_scr_act());
    lv_obj_set_size(btn1, 128, 64);
    lv_obj_set_style_bg_color(btn1, lv_color_hex(0xff0000), 0);
    lv_obj_center(btn1);
    #else
    lv_demo_music();
    #endif
   while (true)
   {
      vTaskDelay(1);
      lv_task_handler();
   }
}
#endif

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

    lv_init();
    lv_port_disp_init();
    timer_init();
    
    xTaskCreate(ledTask, "ledTask", 4096, NULL, 1, NULL);
    xTaskCreate(shellTask, "shellTask", 4096, NULL, 1, NULL);
    xTaskCreate(lvgl_task, "lvglTask", 4096, NULL, 1, NULL);

    while(1)
    {
        vTaskDelay(1);
    }
}
