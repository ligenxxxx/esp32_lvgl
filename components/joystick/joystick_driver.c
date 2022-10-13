#include "driver/i2c.h"
#include "../../i2c/include/i2c_driver.h"
#include "../include/joystick_driver.h"

joystick_t joystick;

esp_err_t joystick_updata(void)
{
    esp_err_t err;
    uint8_t wdata[2];
    uint8_t rdata[5];

    wdata[0] = JOYSTICK_LEFT_X_REG;
    wdata[1] = JOYSTICK_BUTTON_REG;
    err = i2c_master_write_read_device(i2c0_master_port, JOYSTICK_I2C_ADDR, &wdata[0], 1, rdata, 2, I2C0_MASTER_TIMEOUT_MS / portTICK_RATE_MS);
    if(err != ESP_OK)
    {
        joystick.isConnected = 0;
        return err;
    }
    joystick.stick_x = rdata[0];
    joystick.stick_y = rdata[1];

    err = i2c_master_write_read_device(i2c0_master_port, JOYSTICK_I2C_ADDR, &wdata[1], 1, rdata, 5, I2C0_MASTER_TIMEOUT_MS / portTICK_RATE_MS);
    if(err != ESP_OK)
    {
        joystick.isConnected = 0;
        return err;
    }
    joystick.button_stick = rdata[0];
    joystick.button_down = rdata[1];
    joystick.button_up = rdata[2];
    joystick.button_right = rdata[3];
    joystick.button_left = rdata[4];

    joystick.isConnected = 1;

    return err;
}

void joystick_Task(void)
{
    while(1)
    {
        if(joystick_updata() == ESP_OK)
            printf("\r\n%d  x:%d y:%d  up:%d  down:%d  left:%d  right:%d  stick:%d",
                    joystick.isConnected, joystick.stick_x, joystick.stick_y,
                    joystick.button_up, joystick.button_down, joystick.button_left, joystick.button_right,
                    joystick.button_stick);
        else
            printf("\r\njoystick lost");
            
        vTaskDelay(100 / portTICK_RATE_MS);
    }
}