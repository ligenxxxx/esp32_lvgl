#include "esp_system.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "../../main/global.h"
#include "../uart/uart_driver.h"
#include "../led/led_driver.h"
#include "shell.h"

static void parseCommand(char *commandStr, uint8_t commandLen, shell_t *shell)
{
    if (!strncmp(commandStr, "reset", sizeof("reset")))
    {
        esp_restart();
    }
    #if(0)
    else if(!strncmp(commandStr, "xmodem_rx", sizeof("xmodem_rx")))
    {
        shell->outputs((uint8_t *)"\r\nwait xmodem....\r\n", 19);
        shell->xmodem_rx(shell->output, shell->input, shell->LedBlink);
    }
    #endif
}

static void ShellProc(shell_t *shell)
{
    uint8_t isCommand = 0;
    char commandStr[256];
    uint8_t commandLen = 0;
    static uint8_t rdat;

    while (shell->input(&rdat))
    {
        shell->LedBlink();
        switch (shell->status)
        {
        case 0:
            if (rdat >= 0x20 && rdat <= 0x7E) // char
            {
                shell->ibuf[shell->ptr++] = rdat;
            }
            else if (rdat == 0x0A) // Line Feed
            {
                if (shell->ptr)
                {
                    commandLen = shell->ptr;
                    shell->ibuf[shell->ptr++] = '\0';
                    memcpy(commandStr, shell->ibuf, shell->ptr);
                    shell->ptr = 0;
                    isCommand = 1;
                }
                else
                    isCommand = 0;
            }
            else if (rdat == 0x08) // Back Space
            {
                if (shell->ptr)
                {
                    shell->ptr--;
                    shell->output(0x08);
                    shell->output(' ');
                }
            }
            shell->output(rdat);
            break;
        } // switch

        if (rdat == 0x0A)
        {
            if (isCommand)
            {
                shell->outputs((uint8_t *)"cmd:", 4);
                shell->outputs(shell->ibuf, commandLen);
                shell->outputs((uint8_t *)"\r\n", 2);
                isCommand = 0;

                parseCommand(commandStr, commandLen, shell);

                shell->outputs((uint8_t *)"\r\n>", 3);
            }
            else if (!(shell->ptr))
                shell->output('>');
        }
    }
}

/*define shell send byte*/
static void shell_UartTxByte(uint8_t tdat)
{
    uart_TxByte(0, tdat);
}
/*define shell send bytes*/
static void shell_UartTxBytes(uint8_t *tbuf, uint8_t len)
{
    uart_TxBytes(0, tbuf, len);
}

/*define shell receive byte*/
static uint8_t shell_UartRxByte(uint8_t *rdat)
{
    uint8_t ret = 0;
    if (uart_Ready(0))
    {
        *rdat = uart_RxByte(0);
        ret = 1;
    }

    return ret;
}

void shellTask()
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

    #ifdef _DEBUG_MODE
    printf("\r\nshellTask start");
    #endif

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