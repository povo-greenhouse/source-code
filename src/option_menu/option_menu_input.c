#include <msp.h>
#include <ti/devices/msp432p4xx/driverlib/driverlib.h>
#include <stdint.h>
#include "include/option_menu/option_menu_input.h"
#include "include/option_menu/option_menu.h"
#define JOYSTICK_THRESHOLD_UP 10000
#define JOYSTICK_THRESHOLD_DOWN 6000

//at rest 7460-7480
//left:7250-60
//right:7800
#define JOYSTICK_THRESHOLD_LEFT 7400
#define JOYSTICK_THRESHOLD_RIGHT 7550

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
    if(!option_menu_is_enabled) {return;}
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
    if(!option_menu_is_enabled) {return NONE;}
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


void adc_init(){
    /* Configures Pin 6.0 and 4.4 as ADC input(vertical and horizontal joystick) */
    GPIO_setAsPeripheralModuleFunctionInputPin(GPIO_PORT_P6,GPIO_PIN0,GPIO_TERTIARY_MODULE_FUNCTION);
    GPIO_setAsPeripheralModuleFunctionInputPin(GPIO_PORT_P4,GPIO_PIN4,GPIO_TERTIARY_MODULE_FUNCTION);


    //initializing ADC(ADCOSC/64/8)
    ADC14_enableModule();
    ADC14_initModule(ADC_CLOCKSOURCE_ADCOSC, ADC_PREDIVIDER_64, ADC_DIVIDER_8,0);

    //configuring ADC memory with repeat
    ADC14_configureMultiSequenceMode(ADC_MEM0, ADC_MEM1,true);
    ADC14_configureConversionMemory(ADC_MEM0, ADC_VREFPOS_AVCC_VREFNEG_VSS, ADC_INPUT_A15, ADC_NONDIFFERENTIAL_INPUTS);

    ADC14_configureConversionMemory(ADC_MEM1, ADC_VREFPOS_AVCC_VREFNEG_VSS, ADC_INPUT_A9, ADC_NONDIFFERENTIAL_INPUTS);

    //enabling interrupt when a conversion is complete
    ADC14_enableInterrupt(ADC_INT1);

    //Enable interrupts
    Interrupt_enableInterrupt(INT_ADC14);
    //Interrupt_enableMaster();


    ADC14_enableSampleTimer(ADC_AUTOMATIC_ITERATION);

    ADC14_enableConversion();
    ADC14_toggleConversionTrigger();
}

void buttons_init(){
    //BUTTON A INPUT: PIN 5.1

    //BUTTON B INPUT: PIN 5.2
    //BUTTON JOYSTICK SELECT: PIN 4.1

    P5->SEL0 &= ~(BIT1 | BIT2);
    P5->SEL1 &= ~(BIT1 | BIT2);
    P4->SEL0 &= ~BIT1;
    P4->SEL1 &= ~BIT1;

    P5-> DIR &= ~(BIT1 | BIT2);
    P4 -> DIR &= ~(BIT1);

    P5->REN &= (BIT1 | BIT2);
    P4->REN &= (BIT1);

    //P5->IN |= (BIT1 | BIT2);
    //P4->IN |= (BIT1);

    // hope it's pullup
    P4->OUT |=(BIT1 |BIT2);
    P4->OUT |=(BIT1);

    P5->IES &= ~(BIT1 | BIT2);
    P4->IES &= ~(BIT1);

    P5->IE |= (BIT1 | BIT2);
    P4->IE |= (BIT1);
    P5->IFG = 0;
    P4->IFG = 0;
    //TODO: learn what operation I can do s.t ISER doesnt get wiped out everytime :) Maybe + or - ?
    /*
    NVIC->ISER[1] ^=1<<((PORT4_IRQn) &31);
    NVIC->ISER[1] ^=1<<((PORT5_IRQn) &31);
    */
    Interrupt_enableInterrupt(INT_PORT4);
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

    if(P5->IFG & BIT1){
        P5->IFG &= ~BIT1;
        return BUTTON_A;
    }
    if(P5->IFG & BIT2) {
        P5->IFG &= ~BIT2;
        return BUTTON_B;
    }

    return NONE;
}
void PORT4_IRQHandler(void){

    //used to toggle option menu
    if(P4->IFG & BIT1){
                P4->IFG &= ~BIT1;
                option_menu_toggle();

    }
}
void PORT5_IRQHandler(void){
    ControllerInputOption a;
    for(a = get_button_input();a != NONE; a = get_button_input()){
        input_buffer_enqueue(a);
    }
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
void ADC14_IRQHandler(void){
    uint64_t status = ADC14_getEnabledInterruptStatus();
    ADC14_clearInterruptFlag(status);
    if(status & ADC_INT1) {



        joystick_h_result = ADC14_getResult(ADC_MEM0);
        joystick_v_result = ADC14_getResult(ADC_MEM1);
        ControllerInputOption direction = get_joystick_direction(joystick_h_result,joystick_v_result);
        add_to_input_buffer(direction);


    }

}

void init_option_menu_input(){
    init_input_queue();
    buttons_init();
    adc_init();

}
