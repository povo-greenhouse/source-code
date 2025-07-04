/*
 * uart_comm.h
 *
 *  Created on: Jul 4, 2025
 *      Author: riginel
 */

#ifndef INCLUDE_UART_COMMUNICATION_UART_COMM_H_
#define INCLUDE_UART_COMMUNICATION_UART_COMM_H_

#include<ti/drivers/UART.h>
#include <stdint.h>

/*
 * PROTOCOL
 * This is a replacement for the option menu input and the screen since they dont work on this board.
 * input possible messages:
 * - joystick and buttons input
 * - water level sensor
 * - air quality
 */
typedef enum  {
    U_LEFT,
    U_RIGHT,
    U_UP,
    U_DOWN,
    U_A,
    U_B
} Uart_Input_Actions;

Uart_Input_Actions uart_input_from_string(char * buf, size_t len, int32_t * error);

#define UART_READ_BUF_LEN 10
#define UART_WRITE_BUF_LEN 30
extern char uart_read_buf[UART_READ_BUF_LEN];
extern char uart_write_buf[UART_WRITE_BUF_LEN];

UART_Handle uart_init();

void uart_read_callback(UART_Handle handle, void *buf, size_t count);
void uart_write_callback(UART_Handle handle, void * buf, size_t count);


/*
 *
 * HERE A DILEMMA
 * How to handle the reads?
 * i have to call read every time I want some message, and it will call the callback function
 * should I make a periodic task?
 * or should the scheduler always have it in queue?
 * hmm
 * maybe periodic is better
 * but wont i miss some inputs?
 * We'll see
 *
 */


#endif /* INCLUDE_UART_COMMUNICATION_UART_COMM_H_ */
