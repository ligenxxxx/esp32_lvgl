
#include <stdio.h>
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
static lv_obj_t *obj_label;

int angle = -90;
int angle_warning[2] = {-45,45};
uint8_t blink_cnt = 0;
uint8_t is_blinking = 0;

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

uint32_t color_array[13] = {
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
    0x404040, // 0,   0,   0   blink bg color
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

static lv_obj_t *create_instrument_profile(lv_obj_t *parent)
{
    lv_obj_t *obj;

    LV_IMG_DECLARE(image_instrument_profile);
    obj = lv_img_create(parent);
    lv_img_set_src(obj, &image_instrument_profile);
    lv_obj_center(obj);
    lv_obj_set_size(obj, MY_DISP_HOR_RES, MY_DISP_VER_RES);

    return obj;
}

static void angle_clip(void)
{
    if(angle < -90)
        angle = -90;
    else if(angle > 90)
        angle = 90;
}

static void update_blink_status(void)
{
    if(angle <= angle_warning[0] || angle >= angle_warning[1])
            is_blinking = 1;
        else
            is_blinking = 0;

    if(is_blinking)
        blink_cnt ++;
    else
        blink_cnt = 0;
}


static void update_meter_angle(void)
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
}

static void update_meter_color(void)
{
    static uint8_t lst_color_index = 0xff;
    uint8_t i;

    for(i=0; i<12; i++)
    {
        if(angle >= loc[i][0] && angle < loc[i][1])
        {
            if(lst_color_index != i || is_blinking)
            {
                if(blink_cnt & 2)
                {
                    indic[0]->type_data.arc.color =  lv_color_hex(color_array[12]);
                    indic[1]->type_data.arc.color =  lv_color_hex(color_array[12]);
                }
                else
                {
                    indic[0]->type_data.arc.color =  lv_color_hex(color_array[i]);
                    indic[1]->type_data.arc.color =  lv_color_hex(color_array[i]);
                }
                
                lv_obj_invalidate(obj_meter);
                lst_color_index = i;
                break;
            }

        }
    }
}

static void angle_add(void)
{

#if(1)
    angle += 1;
    if(angle > 90)
        angle = -90;
#endif
}

static void update_meter_string(void)
{
    char str[11] = "";
    int angle_abs = (angle < 0) ? (0-angle) : angle;

    sprintf(str, "%d", angle_abs);
    lv_label_set_text(obj_label, str);
}

static void update_meter()
{

    angle_clip();
    update_blink_status();
    update_meter_color();
    update_meter_angle();
    update_meter_string();
    
    angle_add();
}

static lv_obj_t *create_meter(lv_obj_t *parent)
{
    lv_obj_t *meter;

    meter = lv_meter_create(parent);
    lv_obj_set_style_bg_color(meter, lv_color_hex(color_array[12]), 0);
    lv_obj_set_size(meter, 194, 194);
    lv_obj_set_pos(meter, 23,82);

    lv_obj_remove_style(meter, NULL, LV_PART_INDICATOR);

    //left
    scale[0] = lv_meter_add_scale(meter);
    lv_meter_set_scale_range(meter, scale[0], -90, 0, 90, 180);
    lv_meter_set_scale_ticks(meter, scale[0], 7, 2, 8, lv_color_white());
    indic[0] = lv_meter_add_arc(meter,scale[0], 30, lv_color_hex(color_array[5]), 10);
    lv_meter_set_indicator_end_value(meter, indic[0], 0); //locked end value

    //right
    scale[1] = lv_meter_add_scale(meter);
    lv_meter_set_scale_range(meter, scale[1], 0, 90, 90, 270);
    lv_meter_set_scale_ticks(meter, scale[1], 7, 2, 8, lv_color_white());
    indic[1] = lv_meter_add_arc(meter,scale[1], 30, lv_color_hex(color_array[5]), 10);
    lv_meter_set_indicator_end_value(meter, indic[1],90); //locked start value

    return meter;
}

static lv_obj_t *create_label(lv_obj_t *parent)
{
    lv_obj_t *label = lv_label_create(parent);
    lv_obj_set_style_text_color(parent, lv_color_hex(0xffffff), LV_PART_MAIN);
    lv_obj_align(label, LV_ALIGN_CENTER, 0, 40);

    lv_label_set_text(label, "0");

    return label;
}

void ui_main_task(void)
{

    obj_background = create_background(lv_scr_act());

    obj_meter = create_meter(obj_background);

    obj_instrument_profile = create_instrument_profile(obj_background);
    
    obj_label = create_label(obj_background);

    #if(1)
    lv_anim_t a;
    lv_anim_init(&a);
    lv_anim_set_exec_cb(&a, update_meter);
    lv_anim_set_time(&a, 10000);
    lv_anim_set_playback_delay(&a, 0);
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