#include "shell.h"
#include "esp_system.h"

uint8_t rdat;

void parseCommand(char *commandStr, uint8_t commandLen, shell_t *shell)
{
    if (!strncmp(commandStr, "reset", sizeof("reset")))
    {
        esp_restart();
    }
    /*
    else if(!strncmp(commandStr, "xmodem_rx", sizeof("xmodem_rx")))
    {
        shell->outputs((uint8_t *)"\r\nwait xmodem....\r\n", 19);
        shell->xmodem_rx(shell->output, shell->input, shell->LedBlink);
    }
    */
}

void ShellProc(shell_t *shell)
{
    uint8_t isCommand = 0;
    char commandStr[256];
    uint8_t commandLen = 0;

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