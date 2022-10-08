#ifndef __LED_DRIVER_H__
#define __LED_DRIVER_H__

#include "driver/gpio.h"

#define LED0_PIN 2

typedef enum
{
    OFF,
    ON
}led_e;

typedef enum
{
    MODE_IDLE,
    MODE_WORK,
    MODE_BUSY,
    MODE_HARD,
    MODE_ON,
    MODE_OFF,
    MODE_BLINK,
}led_mode_e;

void led_init(void);
uint8_t led_update(uint32_t sysMs);
void led_blink(void);

extern uint8_t led0_mode;
#endif