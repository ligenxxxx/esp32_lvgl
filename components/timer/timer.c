#include "timer.h"
#include "esp_timer.h"
#if(0)
#include "../lvgl/lvgl.h"
static esp_timer_handle_t lvgl_timer_handle = NULL;

static void lv_timer_cb(void *arg)
{
   lv_tick_inc(1);
}

static esp_timer_create_args_t lvgl_timer = {
    .callback = &lv_timer_cb,
    .arg = NULL,
    .name ="lvgl_timer",
    .dispatch_method = ESP_TIMER_TASK
};

void timer_init(void)
{
    esp_err_t err;
    err = esp_timer_create(&lvgl_timer, &lvgl_timer_handle);
    if(err!= ESP_OK)
        printf("\r\nlvgl timer create failed");
    err = esp_timer_start_periodic(lvgl_timer_handle, 1000);
    if(err!= ESP_OK)
        printf("\r\nlvgl timer start failed");
}
#endif