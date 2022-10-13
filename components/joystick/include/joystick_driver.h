#ifndef __JOYSTICK_DRIVER_H__
#define __JOYSTICK_DRIVER_H__

#define JOYSTICK_I2C_ADDR         0x5A
#define JOYSTICK_BASE_REG         0x10
#define JOYSTICK_LEFT_X_REG       0x10
#define JOYSTICK_LEFT_Y_REG       0x11
//#define JOYSTICK_RIGHT_X_REG      0x12
//#define JOYSTICK_RIGHT_Y_REG      0x13

#define BUTOON_BASE               0x20

#define BUTOON_LEFT_REG           0x24
#define BUTOON_RIGHT_REG          0x23
#define BUTOON_UP_REG             0x22
#define BUTOON_DOWN_REG           0x21
#define JOYSTICK_BUTTON_REG       0x20

#if(0)
#define	PRESS_DOWN                0
#define	PRESS_UP                  1
#define	PRESS_REPEAT              2
#define	SINGLE_CLICK              3
#define	DOUBLE_CLICK              4
#define	LONG_PRESS_START          5
#define	LONG_PRESS_HOLD           6
#define	number_of_event           7
#define	NONE_PRESS                8
#endif

typedef enum
{
    BTN_PRESS_DOWN,
    BTN_PRESS_UP,
    BTN_PRESS_REPEAT,
    BTN_SINGLE_CLICK,
    BTN_DOUBLE_CLICK,
    BTN_LONG_PRESS_START,
    BTN_LONG_PRESS_HOLD,
    BTN_number_of_event,
    BTN_NONE_PRESS,
} button_e;

typedef struct
{
    bool isConnected;
    uint8_t stick_x;
    uint8_t stick_y;
    button_e button_up;
    button_e button_down;
    button_e button_left;
    button_e button_right;
    button_e button_stick;
} joystick_t;

void joystick_Task(void);

#endif