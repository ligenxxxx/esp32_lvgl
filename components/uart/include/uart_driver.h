#ifndef __UART_DRIVER_H__
#define __UART_DRIVER_H__
#include <stdint.h>
#include "driver/uart.h"

#define UART0_TX_PIN 1
#define UART0_RX_PIN 3
//#define UART1_TX_PIN 10
//#define UART1_RX_PIN 9
#define UART2_TX_PIN 17
#define UART2_RX_PIN 16

#define UART0_BAUD 115200
#define UART2_BAUD 115200

void uart0_init(void);
void uart2_init(void);
uint8_t uart_Ready(uint8_t uart_num);
uint8_t uart_RxByte(uint8_t uart_num);
void uart_TxByte(uint8_t uart_num, uint8_t tdat);
void uart_TxBytes(uint8_t uart_num, uint8_t *tbuf, uint8_t length);

extern const uart_port_t uart0_num;
extern const uart_port_t uart2_num;

#endif