#include "uart_driver.h"

uart_port_t const uart0_num = UART_NUM_0;
uart_port_t const uart2_num = UART_NUM_2;

uart_config_t uart0_config = {
    .baud_rate = UART0_BAUD,
    .data_bits = UART_DATA_8_BITS,
    .parity = UART_PARITY_DISABLE,
    .stop_bits = UART_STOP_BITS_1,
    .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
    .rx_flow_ctrl_thresh = 122,
};
uart_config_t uart2_config = {
    .baud_rate = UART2_BAUD,
    .data_bits = UART_DATA_8_BITS,
    .parity = UART_PARITY_DISABLE,
    .stop_bits = UART_STOP_BITS_1,
    .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
    .rx_flow_ctrl_thresh = 122,
};

const int uart0_txbuffer_size = 1024;
const int uart0_rxbuffer_size = 1024;
const int uart2_txbuffer_size = 1024;
const int uart2_rxbuffer_size = 1024;

QueueHandle_t uart0_queue;
QueueHandle_t uart2_queue;

void uart0_init(void)
{
    // Configure UART parameters
    ESP_ERROR_CHECK(uart_param_config(uart0_num, &uart0_config));
    ESP_ERROR_CHECK(uart_set_pin(uart0_num, UART0_TX_PIN, UART0_RX_PIN, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE));
    ESP_ERROR_CHECK(uart_driver_install(uart0_num, uart0_rxbuffer_size,
                                        uart0_txbuffer_size, 10, &uart0_queue, 0));
}

void uart2_init(void)
{
    // Configure UART parameters
    ESP_ERROR_CHECK(uart_param_config(uart2_num, &uart2_config));
    ESP_ERROR_CHECK(uart_set_pin(uart2_num, UART2_TX_PIN, UART2_RX_PIN, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE));
    ESP_ERROR_CHECK(uart_driver_install(uart2_num, uart2_rxbuffer_size,
                                        uart2_txbuffer_size, 10, &uart2_queue, 0));
}

uint8_t uart_Ready(uint8_t uart_num)
{
    int length = 0;

    if (uart_num == 0)
        ESP_ERROR_CHECK(uart_get_buffered_data_len(uart0_num, (size_t *)&length));
    else if (uart_num == 1)
        ESP_ERROR_CHECK(uart_get_buffered_data_len(uart0_num, (size_t *)&length));

    return (length != 0);
}

uint8_t uart_RxByte(uint8_t uart_num)
{
    uint8_t rdat = 0;

    if (uart_num == 0)
        uart_read_bytes(uart0_num, &rdat, 1, 100);
    else
        uart_read_bytes(uart2_num, &rdat, 1, 100);

    return rdat;
}

void uart_TxByte(uint8_t uart_num, uint8_t tdat)
{
    if (uart_num == 0)
        uart_write_bytes(uart0_num, (const char *)&tdat, 1);
    else
        uart_write_bytes(uart2_num, (const char *)&tdat, 1);
}

void uart_TxBytes(uint8_t uart_num, uint8_t *tbuf, uint8_t length)
{
    if (uart_num == 0)
        uart_write_bytes(uart0_num, (const char *)tbuf, length);
    else
        uart_write_bytes(uart2_num, (const char *)tbuf, length);
}