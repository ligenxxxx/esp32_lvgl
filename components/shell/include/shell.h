#ifndef __SHELL_H__
#define __SHELL_H__
#include <stdint.h>
#include <stdio.h>
#include <memory.h>

typedef void (*output_t)(uint8_t tdat);
typedef void (*outputs_t)(uint8_t *tbuf, uint8_t len);
typedef uint8_t (*input_t)(uint8_t *rdat);
typedef void (*LedBlink_t)(void);
typedef uint32_t (*xmodem_rx_t)(output_t output, input_t input, LedBlink_t LedBlink);

typedef struct
{
    output_t output;
    outputs_t outputs;
    input_t input;
    uint8_t status;
    uint8_t *ibuf;
    uint8_t ptr;
    LedBlink_t LedBlink;
    xmodem_rx_t xmodem_rx;
} shell_t;

void ShellProc(shell_t *shell);
#endif