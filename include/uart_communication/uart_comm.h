/*
 * uart_comm.h
 *
 *  Created on: Jul 4, 2025
 *      Author: riginel
 */

#ifndef INCLUDE_UART_COMMUNICATION_UART_COMM_H_
#define INCLUDE_UART_COMMUNICATION_UART_COMM_H_

#include<ti/devices/msp432p4xx/driverlib/driverlib.h>
#include <stdint.h>
#include <stdbool.h>
#define UART_BUF_LEN 256
#define READ_BUF_LEN 64
typedef enum __RxMessageType {
    CONTROLLER,
    WATER,
    AIR
}RxMessageType;
void RMT_to_string(uint8_t * buffer, RxMessageType type);

RxMessageType RMT_from_string(const uint8_t * str,uint16_t len);

void parse_message(const uint8_t * buffer,uint16_t len);

typedef struct {
    uint8_t arr[UART_BUF_LEN];
    volatile uint16_t write_index;
    volatile uint16_t read_index;
} UART_Buffer;

typedef void(*fp_rx_callback) (uint8_t);
typedef void(*fp_tx_callback) (void);

// UART Management Struct
typedef struct {
    UART_Buffer tx_buff;
    volatile bool tx_busy;        // Transmission in progress

    UART_Buffer rx_buff;
    volatile bool rx_overflow;

    char read_buf[READ_BUF_LEN];
    fp_tx_callback tx_complete_callback;    // TX complete callback
    fp_rx_callback rx_data_callback; // RX data callback
} UART_Context;

volatile UART_Context uart_ctx;


/*
 * PROTOCOL
 * This is a replacement for the option menu input and the screen since they dont work on this board.
 * input possible messages:
 * - joystick and buttons input
 * - water level sensor
 * - air quality
 */


void  uart_init();
bool UART_write(const uint8_t *data, uint16_t length, void (*callback)(void));
uint16_t UART_read(uint8_t * buffer, uint16_t max_length);


void handle_msg(void);

#endif /* INCLUDE_UART_COMMUNICATION_UART_COMM_H_ */
