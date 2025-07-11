#include <msp.h>
#include <ti/devices/msp432p4xx/driverlib/driverlib.h>
#include "uart_communication/uart_comm.h"
#include "option_menu/option_menu_input.h"
#include "scheduling/scheduler.h"
#include <string.h>
#include<stdbool.h>



void uart_init(){
    //setting up uart context
    uart_ctx.tx_buff.write_index = 0;
    uart_ctx.tx_buff.read_index = 0;
    uart_ctx.tx_busy = false;

    uart_ctx.rx_buff.write_index = 0;
    uart_ctx.rx_buff.read_index = 0;
    uart_ctx.rx_overflow = false;

    //GPIO CONFIGURATION(P1.2,P1.3)
    GPIO_setAsPeripheralModuleFunctionInputPin(
          GPIO_PORT_P1,
          GPIO_PIN2 | GPIO_PIN3,
          GPIO_PRIMARY_MODULE_FUNCTION
    );

    eUSCI_UART_ConfigV1 uart_config = {
         EUSCI_A_UART_CLOCKSOURCE_SMCLK,
         19,
         8,
         85,
         EUSCI_A_UART_NO_PARITY,
         EUSCI_A_UART_MSB_FIRST,
         EUSCI_A_UART_ONE_STOP_BIT,
         EUSCI_A_UART_MODE,
         EUSCI_A_UART_OVERSAMPLING_BAUDRATE_GENERATION,
         EUSCI_A_UART_8_BIT_LEN
    };
    //initialize and enable uart
    UART_initModule(EUSCI_A0_BASE, &uart_config);
    UART_enableModule(EUSCI_A0_BASE);

    // enable TX and RX interrupts
    UART_enableInterrupt(EUSCI_A0_BASE,
                             EUSCI_A_UART_TRANSMIT_INTERRUPT | EUSCI_A_UART_RECEIVE_INTERRUPT);

    Interrupt_enableInterrupt(INT_EUSCIA0);
    Interrupt_enableMaster();

}





bool uart_buff_is_full(UART_Buffer * buff){
    if ((buff->write_index + 1) % UART_BUF_LEN ==
            buff->read_index) {
            return 1;
        }
    return 0;
}
bool uart_buff_is_empty(UART_Buffer * buff){
    if(buff->write_index == buff->read_index){
        return 1;
    }
    return 0;
}
int uart_buff_enqueue(UART_Buffer * buff, uint8_t ch) {

    if(uart_buff_is_full(buff)){return 0;}
    buff->arr[buff->write_index] = ch;
    buff->write_index = (buff->write_index + 1) % UART_BUF_LEN;
    return 1;
}
inline uint16_t buff_available_space(UART_Buffer * buff){
    if(buff->write_index >= buff->read_index){
        return UART_BUF_LEN - (buff->write_index - buff->read_index);

    }
    return buff->read_index - buff->write_index;
}
// dequeues the last char
uint8_t uart_buff_dequeue(UART_Buffer * buff, int * error) {
    if(uart_buff_is_empty(buff)){
        *error = 1;
        return 0;
    }
    uint8_t ret = buff->arr[buff->read_index];
    buff->read_index = (buff->read_index + 1) % UART_BUF_LEN;
    return ret;
}


void EUSCIA0_IRQHandler(void) {
    uint32_t status = MAP_UART_getEnabledInterruptStatus(EUSCI_A0_BASE);
    MAP_UART_clearInterruptFlag(EUSCI_A0_BASE, status);

    // tx

    if(status & EUSCI_A_UART_TRANSMIT_INTERRUPT_FLAG){
        //send next char in queue
        int error = 0;
        uint8_t next_ch = uart_buff_dequeue(&uart_ctx.tx_buff,&error);
        UART_Context * uart_ctx_ptr = &uart_ctx;
        if(error == 0){
            UART_transmitData(EUSCI_A0_BASE, next_ch);
            printf("transmitted %c\n",next_ch);
        }else{
            //buffer empty: disable tx interrupt
            MAP_UART_disableInterrupt(EUSCI_A0_BASE,EUSCI_A_UART_TRANSMIT_INTERRUPT);
            uart_ctx.tx_busy = false;

            //invoke tx completion callback
            if(uart_ctx.tx_complete_callback){
                uart_ctx.tx_complete_callback();

            }
        }
    }
    // rx

    if(status & EUSCI_A_UART_RECEIVE_INTERRUPT_FLAG){
        uint8_t rx_data = MAP_UART_receiveData(EUSCI_A0_BASE);

        if(uart_buff_enqueue(&uart_ctx.rx_buff,rx_data)){
            if(rx_data ==0){
                //end of message, handle the input
                Interrupt_disableMaster();
                STask t = {
                           handle_msg,
                           0,
                           0,
                           true
                };
                enqueue_task(&t);
                scheduler_state = AWAKE;
                Interrupt_enableMaster();
            }
            if(uart_ctx.rx_data_callback){
                uart_ctx.rx_data_callback(rx_data);
            }

        }else {
            //overflow
            uart_ctx.rx_overflow = true;
        }
    }
}

void tx_complete_callback(void){

}
void rx_data_callback(uint8_t ch){
    //message completed, allow read
    if(ch == 0){

    }
}

bool UART_write(const uint8_t *data, uint16_t length, void (*callback)(void)){
    // disable interrupts for atomic access
    uint32_t int_status = Interrupt_disableMaster();
    //check buffer space
    uint16_t free_space = buff_available_space(&uart_ctx.tx_buff);
    if(free_space < length){
        Interrupt_enableMaster();
        return false;
    }
    uint16_t i = 0;
    for(i = 0;i < length;i++){

        uart_buff_enqueue(&uart_ctx.tx_buff,data[i]);
        if(data[i]==0){
                    break;
        }
    }
    uart_ctx.tx_complete_callback = callback;

    //start transmission
    if(!uart_ctx.tx_busy){
        uart_ctx.tx_busy = true;
        UART_enableInterrupt(EUSCI_A0_BASE,EUSCI_A_UART_TRANSMIT_INTERRUPT);
        // Trigger first transmission
        int32_t first_ch = uart_buff_dequeue(&uart_ctx.tx_buff,NULL);

        UART_transmitData(EUSCI_A0_BASE, first_ch);

    }
    Interrupt_enableMaster();
    return true;
}

uint16_t UART_read(uint8_t * buffer, uint16_t max_length){
    uint32_t int_status = Interrupt_disableMaster();
    uint16_t bytes_read = 0;
    uint16_t chars_available = UART_BUF_LEN - buff_available_space(&uart_ctx.rx_buff);
    while(bytes_read < max_length && bytes_read < chars_available ){
        uint8_t next_ch = uart_buff_dequeue(&uart_ctx.rx_buff,NULL);
        buffer[bytes_read++] = next_ch;
        if(next_ch == 0){
            break;
        }

    }
    Interrupt_enableMaster();
    return bytes_read;
}

void RMT_to_string(uint8_t * buffer, RxMessageType type){
    switch(type){
    case CONTROLLER:
        strcpy(buffer,"CONTROLLER");
        break;
    case WATER:
        strcpy(buffer,"WATER");
    case AIR:
        strcpy(buffer,"AIR");
    }
}

RxMessageType RMT_from_string(const uint8_t * str,uint16_t len){
    if(strncmp(str,"CONTROLLER",len)){
        return CONTROLLER;
    }
    if(strncmp(str,"WATER",len)){
            return WATER;
        }
    if(strncmp(str,"AIR",len)){
            return AIR;
        }
}

void handle_msg(){
    UART_read(uart_ctx.read_buf,READ_BUF_LEN);

    parse_msg(uart_ctx.read_buf,READ_BUF_LEN);

}

void handle_controller_msg(const char * buff,uint16_t len ){
    //parse the thing and add it to the queue
    ControllerInputOption opt = option_input_from_str(buff,len);
    if(opt == JOYSTICK_SELECT){

        option_menu_toggle();
        return;
    }
   // Interrupt_disableMaster();
    input_buffer_enqueue(opt);
   // Interrupt_enableMaster();
}

void handle_water_msg(const char * buff, uint16_t len){
    int32_t val = atoi(buff);
    //TODO
}
void handle_air_msg(const char * buff, uint16_t len){
    int32_t val = atoi(buff);

}
void parse_msg(const uint8_t * buffer,uint16_t len){


    //find the :
    char *  colon_pos = strchr(buffer, ':');
    if(colon_pos < buffer && colon_pos > buffer + len){
        return;
    }
    uint16_t index = colon_pos - buffer;
    char * value_str = colon_pos + 1;
    RxMessageType type = RMT_from_string(buffer,index);
    switch(type){
        case CONTROLLER:
            handle_controller_msg(value_str, len-index);
            break;
        case WATER:
            handle_water_msg(value_str, len-index);
            break;
        case AIR:
            handle_air_msg(value_str,len-index);
            break;
    }
    return;


}


