#include "ui_main.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "../../lvgl/lvgl.h"
#include "../../lvgl/demos/lv_demos.h"

static lv_obj_t *obj_background; 
static lv_obj_t *obj_instrument_profile;
static lv_obj_t *obj_meter;
static lv_meter_scale_t *scale[2];
static lv_meter_indicator_t *indic[2];

int angle = -90;
const int16_t loc[12][2] = {
    {-90, -60},
    {-60, -50},
    {-50, -45},
    {-45, -40},
    {-40, -30},
    {-30,  0 },
    { 0 ,  30},
    { 30,  40},
    { 40,  45},
    { 45,  50},
    { 50,  60},
    { 60,  90},
};

uint32_t color_array[12] = {
    0xff0d0d, // 255, 13,  13
    0xff550d, // 255, 85,  13
    0xff980d, // 255, 152, 13
    0xfff90d, // 255, 249, 13
    0x58ff0d, // 158, 255, 13
    0x31ff0d, // 49,  255, 13
    0x31ff0d, // 49,  255, 13
    0x58ff0d, // 158, 255, 13
    0xfff90d, // 255, 249, 13
    0xff980d, // 255, 152, 13
    0xff550d, // 255, 85,  13
    0xff0d0d, // 255, 13,  13
};

static lv_obj_t *create_background(lv_obj_t *parent)
{
    lv_obj_t *obj;

    LV_IMG_DECLARE(image_background);
    obj = lv_img_create(parent);
    lv_img_set_src(obj, &image_background);
    lv_obj_center(obj);
    lv_obj_set_size(obj, MY_DISP_HOR_RES, MY_DISP_VER_RES);

    return obj;
}

static lv_obj_t *creatr_imstrument_profile(lv_obj_t *parent)
{
    lv_obj_t *obj;

    LV_IMG_DECLARE(image_instrument_profile);
    obj = lv_img_create(parent);
    lv_img_set_src(obj, &image_instrument_profile);
    lv_obj_center(obj);
    lv_obj_set_size(obj, MY_DISP_HOR_RES, MY_DISP_VER_RES);

    return obj;
}

static void set_angle_value()
{

    if(angle <= 0)
    {
        lv_meter_set_indicator_end_value(obj_meter, indic[1],0);
        lv_meter_set_indicator_start_value(obj_meter, indic[0], angle);
    }
    else if(angle > 0)
    {
        lv_meter_set_indicator_start_value(obj_meter, indic[0], 0);
        lv_meter_set_indicator_end_value(obj_meter, indic[1],angle);
    }
    #if(0)
    else
    {
        lv_meter_set_indicator_start_value(obj_meter, indic[0], -3);
        lv_meter_set_indicator_end_value(obj_meter, indic[1],3);
    }
    #endif


#if(1)
    angle += 1;
    if(angle >90)
        angle = -90;
#endif
}

static lv_obj_t *create_meter(lv_obj_t *parent)
{
    lv_obj_t *meter;

    meter = lv_meter_create(parent);
    lv_obj_set_size(meter, 194, 194);
    lv_obj_set_pos(meter, 23,82);

    lv_obj_remove_style(meter, NULL, LV_PART_INDICATOR);

    //left
    scale[0] = lv_meter_add_scale(meter);
    lv_meter_set_scale_range(meter, scale[0], -90, 0, 90, 180);
    lv_meter_set_scale_ticks(meter, scale[0], 7, 2, 8, lv_color_black());
    indic[0] = lv_meter_add_arc(meter,scale[0], 30, lv_color_hex(color_array[5]), 10);
    //lv_meter_set_indicator_start_value(meter, indic[0], -90);
    lv_meter_set_indicator_end_value(meter, indic[0], 0); //locked end value
    
    #if(0)
    lv_meter_indicator_t * indicc = _lv_ll_ins_head(&((lv_meter_t *)meter)->indicator_ll);
    indicc->type_data.arc.color = lv_color_hex(color_array[0]);
    lv_obj_invalidate(meter);
    #endif

    //right
    scale[1] = lv_meter_add_scale(meter);
    lv_meter_set_scale_range(meter, scale[1], 0, 90, 90, 270);
    lv_meter_set_scale_ticks(meter, scale[1], 7, 2, 8, lv_color_black());
    indic[1] = lv_meter_add_arc(meter,scale[1], 30, lv_color_hex(color_array[5]), 10);
    //lv_meter_set_indicator_start_value(meter, indic[1], 0); //locked start value
    lv_meter_set_indicator_end_value(meter, indic[1],90);

    //set_angle_value();
    return meter;
}

void ui_main_task(void)
{

    obj_background = create_background(lv_scr_act());

    obj_meter = create_meter(obj_background);

    obj_instrument_profile = creatr_imstrument_profile(obj_background);

    //i = 5;
    //select_arc_area(i, 1);
    #if(1)
    lv_anim_t a;
    lv_anim_init(&a);
    lv_anim_set_exec_cb(&a, set_angle_value);
    lv_anim_set_time(&a, 10000);
    lv_anim_set_playback_delay(&a, 100);
    lv_anim_set_playback_time(&a, 0);
    lv_anim_set_repeat_delay(&a, 0);
    lv_anim_set_repeat_count(&a, LV_ANIM_REPEAT_INFINITE);
    lv_anim_start(&a);
    #endif
    
   while (1)
   {
      vTaskDelay(1);
      lv_task_handler();
   }
}