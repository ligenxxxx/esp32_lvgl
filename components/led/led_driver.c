#include "led_driver.h"
uint8_t led0_status;
uint8_t led0_mode;

void led_init(void)
{
    gpio_reset_pin(LED0_PIN);
    gpio_set_direction(LED0_PIN, GPIO_MODE_OUTPUT);
    led0_status = OFF;
    gpio_set_level(LED0_PIN,led0_status);
    led0_mode = MODE_IDLE;
}

void led_blink(void)
{
    led0_status = 1 - led0_status;
    gpio_set_level(LED0_PIN,led0_status);
}

void led_turn(uint8_t op)
{
    led0_status = op;
    gpio_set_level(LED0_PIN,led0_status);
}

uint8_t led_update(uint32_t sysMs)
{
    static uint32_t lstMs = 0;
    uint8_t ret = 0;
    
    switch(led0_mode)
    {
        case MODE_IDLE:
            if(sysMs - lstMs > 1000)
            {
                lstMs = sysMs;
                led_blink();
                ret = 1;
            }
        break;

        case MODE_WORK:
            if(sysMs - lstMs > 500)
            {
                lstMs = sysMs;
                led_blink();
                ret = 1;
            }
        break;

        case MODE_BUSY:
            if(sysMs - lstMs > 200)
            {
                lstMs = sysMs;
                led_blink();
                ret = 1;
            }
        break;

        case MODE_HARD:
            if(sysMs - lstMs > 100)
            {
                lstMs = sysMs;
                led_blink();
                ret = 1;
            }
        break;

        case MODE_ON:
            if(led0_status == OFF)
            {
                led_turn(ON);
                ret = 1;
            }
        break;

        case MODE_OFF:
            if(led0_status == ON)
            {
                led_turn(OFF);
                ret = 1;
            }
        break;

        default:
        break;
    }

    return ret;

}