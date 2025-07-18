#include <msp.h>
#include <ti/devices/msp432p4xx/driverlib/driverlib.h>
#include <stdint.h>
#include "option_menu/option_menu_input.h"
#include "option_menu/option_menu.h"
#include "adc/adc.h"

#define JOYSTICK_THRESHOLD_UP 12000
#define JOYSTICK_THRESHOLD_DOWN 6000

//at rest 7460-7480
//left:7250-60
//right:7800
#define JOYSTICK_THRESHOLD_LEFT 9800
#define JOYSTICK_THRESHOLD_RIGHT 10300

#define BUTTON_PORT P5

#define BUTTON_A_PIN BIT1
#define BUTTON_B_PIN BIT2

uint16_t joystick_h_result = 0, joystick_v_result= 0;




// initializes the task queue
void init_input_queue(){
    input_buffer.read_index = 0;
    input_buffer.write_index = 0;
}

void option_menu_enable_interrupts(){
    Interrupt_enableInterrupt(INT_ADC14);
    Interrupt_enableInterrupt(INT_PORT4);
    Interrupt_enableInterrupt(INT_PORT5);
}
void option_menu_disable_interrupts(){
    Interrupt_disableInterrupt(INT_ADC14);
    Interrupt_disableInterrupt(INT_PORT4);
    Interrupt_disableInterrupt(INT_PORT5);
}


//enqueues the given task
int input_buffer_enqueue(ControllerInputOption input){

    option_menu_disable_interrupts();


    if ((input_buffer.write_index + 1) % INPUT_QUEUE_CAPACITY ==
            input_buffer.read_index) {
        option_menu_enable_interrupts();

            return 0;
        }
        input_buffer.arr[input_buffer.write_index] = input;
        input_buffer.write_index = (input_buffer.write_index + 1) % INPUT_QUEUE_CAPACITY;
        option_menu_enable_interrupts();
        return 1;

}
//dequeues the last task, returning it
ControllerInputOption input_buffer_dequeue(){

    option_menu_disable_interrupts();
    if (input_buffer.read_index == input_buffer.write_index) {
            option_menu_enable_interrupts();
            return NONE;
        }
        ControllerInputOption ret = input_buffer.arr[input_buffer.read_index];
        input_buffer.read_index = (input_buffer.read_index + 1) % INPUT_QUEUE_CAPACITY;
        option_menu_enable_interrupts();
        return ret;
}
void clear_input_queue(){
    ControllerInputOption a;
    for(a = input_buffer_dequeue(); a != NONE; a= input_buffer_dequeue());
}




void buttons_init(){

    //BUTTON A INPUT: PIN 5.1

    //BUTTON B INPUT: PIN 5.2
    //BUTTON JOYSTICK SELECT: PIN 4.1

    BUTTON_PORT->SEL0 &= ~(BUTTON_A_PIN| BUTTON_B_PIN);
    BUTTON_PORT->SEL1 &= ~(BUTTON_A_PIN | BUTTON_B_PIN);

    BUTTON_PORT-> DIR &= ~(BUTTON_A_PIN | BUTTON_B_PIN);


    P5->REN &= (BUTTON_A_PIN | BUTTON_B_PIN);


    //P5->IN |= (BIT1 | BIT2);
    //P4->IN |= (BIT1);



    BUTTON_PORT->IES &= ~(BUTTON_A_PIN| BUTTON_B_PIN);


    BUTTON_PORT->IE |= (BUTTON_A_PIN |BUTTON_B_PIN);

    BUTTON_PORT->IFG = 0;

    //TODO: learn what operation I can do s.t ISER doesnt get wiped out everytime :) Maybe + or - ?
    /*
    NVIC->ISER[1] ^=1<<((PORT4_IRQn) &31);
    NVIC->ISER[1] ^=1<<((PORT5_IRQn) &31);
    */
    Interrupt_enableInterrupt(INT_PORT5);

}
ControllerInputOption get_joystick_direction(uint16_t horizontal,uint16_t vertical){
    if(horizontal>= JOYSTICK_THRESHOLD_RIGHT){
        return RIGHT;
    }
    if(horizontal <= JOYSTICK_THRESHOLD_LEFT){
        return LEFT;
    }
    if(vertical >= JOYSTICK_THRESHOLD_UP){
        return UP;
    }
    if(vertical <= JOYSTICK_THRESHOLD_DOWN){
        return DOWN;
    }
    return NONE;
}
ControllerInputOption get_button_input(){

    if(BUTTON_PORT->IFG & BUTTON_A_PIN){
        BUTTON_PORT->IFG &= ~BUTTON_A_PIN;
        return BUTTON_A;
    }
    if(BUTTON_PORT->IFG & BUTTON_B_PIN) {
        BUTTON_PORT->IFG &= ~BUTTON_B_PIN;
        return BUTTON_B;
    }

    return NONE;
}

void PORT5_IRQHandler(void){
    ControllerInputOption a;
    Interrupt_disableMaster();
    for(a = get_button_input();a != NONE; a = get_button_input()){

        input_buffer_enqueue(a);
    }
    Interrupt_enableMaster();
}


void add_to_input_buffer(ControllerInputOption input){
    /*
                Hopefully not necessary: check if input is the same as before
    */
    static ControllerInputOption last_input = NONE;
    if(input != NONE && input != last_input){
        input_buffer_enqueue(input);
    }
    last_input = input;
    return;
}


void handle_joystick_interrupt(uint64_t status){


        joystick_h_result = ADC14_getResult(X_AXIS_MEM);
        joystick_v_result = ADC14_getResult(Y_AXIS_MEM);
     //   printf("x axis: %d\n",joystick_h_result);
       // printf("y axis: %d\n",joystick_v_result);
        ControllerInputOption direction = get_joystick_direction(joystick_h_result,joystick_v_result);
        Interrupt_disableMaster();
        add_to_input_buffer(direction);
        Interrupt_enableMaster();


}


ControllerInputOption option_input_from_str(char * buf, uint16_t len){
    if(strncmp(buf,"UP",2)==0){
            return UP;
        }
        if(strncmp(buf,"DOWN",4)==0){
                return DOWN;
            }
        if(strncmp(buf,"LEFT",4)==0){
                return LEFT;
            }
        if(strncmp(buf,"RIGHT",5)==0){
                return RIGHT;
            }
        if(strncmp(buf,"BUTTON_A",8)==0){
                return BUTTON_A;
            }
        if(strncmp(buf,"BUTTON_B",8)==0){
                return BUTTON_B;
            }
        if((strncmp(buf,"JOYSTICK_SELECT",15) ==0) || (strncmp(buf,"SELECT",6)==0) ){
            return JOYSTICK_SELECT;
        }
        return NONE;
}


void init_option_menu_input(){
    init_input_queue();
    buttons_init();


}
