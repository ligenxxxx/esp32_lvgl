#include "shellTask.h"
#include "uart_driver.h"
//#include "esp32_devkit_v1.h"
#include "led_driver.h"
#include "shell.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "global.h"

void shell_UartTxByte(uint8_t tdat)
{
    uart_TxByte(0, tdat);
}

void shell_UartTxBytes(uint8_t *tbuf, uint8_t len)
{
    uart_TxBytes(0, tbuf, len);
}

uint8_t shell_UartRxByte(uint8_t *rdat)
{
    uint8_t ret = 0;
    if (uart_Ready(0))
    {
        *rdat = uart_RxByte(0);
        ret = 1;
    }

    return ret;
}

void shellTask(void)
{
    uint8_t shell_buf[256];
    shell_t shell;
    shell.output = shell_UartTxByte;
    shell.outputs = shell_UartTxBytes;
    shell.input = shell_UartRxByte;
    shell.ibuf = shell_buf;
    shell.ptr = 0;
    shell.status = 0;
    shell.LedBlink = led_blink;

    vTaskDelay(1000/portTICK_PERIOD_MS);
    printf("\r\nFW_VERSION: %s", FW_VERSION);
    printf("\r\nBUILD TIME: %s %s", __DATE__, __TIME__);
    printf("\r\n>");
    
    while (1)
    {
        ShellProc(&shell);
        vTaskDelay(1);
    }
}