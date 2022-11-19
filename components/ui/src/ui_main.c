#include "ui_main.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "../../lvgl/lvgl.h"
#include "../../lvgl/demos/lv_demos.h"
void ui_main_task()
{
    #if(1)
    LV_IMG_DECLARE(image_background);
    LV_IMG_DECLARE(image_instrument_profile);
    lv_obj_t *img_background = lv_img_create(lv_scr_act());
    lv_img_set_src(img_background, &image_background);
    lv_obj_center(img_background);
    lv_obj_set_size(img_background, MY_DISP_HOR_RES, MY_DISP_VER_RES);
    
    lv_obj_t *img_instrument_profile = lv_img_create(lv_scr_act());
    lv_img_set_src(img_instrument_profile, &image_instrument_profile);
    lv_obj_center(img_instrument_profile);
    lv_obj_set_size(img_instrument_profile, MY_DISP_HOR_RES, MY_DISP_VER_RES);
    #else
    lv_demo_music();
    #endif
   while (1)
   {
      vTaskDelay(1);
      lv_task_handler();
   }
}