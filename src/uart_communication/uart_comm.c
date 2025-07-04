#include <msp.h>
#include <ti/drivers/UART.h>
#include "uart_communication/uart_comm.h"
#include <string.h>
char read_buf[READ_BUF_LEN];


void uart_read_callback(UART_Handle handle, void *buf, size_t count);
UART_Handle uart_init(){
    //One time init of UART
    UART_init();

    //Init UART parameters
    UART_Params params;
    UART_Params_init(&params);

    //for now let's handle the read mode, reading the input

    params.baudRate = 9600;
    params.readMode = UART_MODE_CALLBACK;
    params.writeMode = UART_MODE_CALLBACK;
    params.readCallback =uart_read_callback;
    params.writeCallback =uart_write_callback;
    //open the UART
    UART_Handle uart = UART_open(CONFIG_UART0, &params);



}

void uart_read_callback(UART_Handle handle, void *buf, size_t count){

}
void uart_write_callback(UART_Handle handle, void *buf, size_t count){

}
Uart_Input_Actions uart_input_from_string(char * buf,size_t len,int * error){
    *error = 0;
    if(strcmp(buf,"LEFT")){
        return U_LEFT;

    }
    if(strcmp(buf,"RIGHT")){
            return U_RIGHT;

        }
    if(strcmp(buf,"UP")){
            return U_UP;

        }
    if(strcmp(buf,"DOWN")){
            return U_DOWN;

        }
    if(strcmp(buf,"A")){
            return U_A;

        }
    if(strcmp(buf,"B")){
            return U_B;

        }
    *error = 1;
    //the first one
    return U_LEFT;
}
