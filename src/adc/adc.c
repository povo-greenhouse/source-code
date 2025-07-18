#include "adc/adc.h"
#include "stdint.h"
#include <ti/devices/msp432p4xx/driverlib/driverlib.h>
#include "option_menu/option_menu_input.h"
#include "water_management/water_reading.h"
#include "environment_systems/air_quality.h"
void adc_init(){
    //first configure the pins
        // Configures Pin 6.0 and 4.4 as ADC input(vertical and horizontal joystick)
        GPIO_setAsPeripheralModuleFunctionInputPin(Y_AXIS_PORT,Y_AXIS_PIN,GPIO_TERTIARY_MODULE_FUNCTION);
        GPIO_setAsPeripheralModuleFunctionInputPin(X_AXIS_PORT,X_AXIS_PIN,GPIO_TERTIARY_MODULE_FUNCTION);
        // Configure GPIO pin P4.3 as analog input for the MQ135 sensor to read the analog voltage output
        GPIO_setAsPeripheralModuleFunctionInputPin(AIR_SENSOR_PORT,
                                                      AIR_SENSOR_PIN,
                                                      GPIO_TERTIARY_MODULE_FUNCTION);
        // Configure pin 5.4 and 5.5 for the tank and reservoire
        GPIO_setAsPeripheralModuleFunctionInputPin(TANK_SENSOR_PORT,
                                                      TANK_SENSOR_PIN ,
                                                      GPIO_TERTIARY_MODULE_FUNCTION);
        GPIO_setAsPeripheralModuleFunctionInputPin(RESERVOIRE_SENSOR_PORT,
                                                             RESERVOIRE_SENSOR_PIN ,
                                                             GPIO_TERTIARY_MODULE_FUNCTION);
        //initializing ADC(ADCOSC/64/8)
        ADC14_enableModule();
        ADC14_initModule(ADC_CLOCKSOURCE_ADCOSC, ADC_PREDIVIDER_64, ADC_DIVIDER_8,0);

        //configuring ADC memory with repeat
        ADC14_configureMultiSequenceMode(ADC_MEM0, ADC_MEM4,true);
        ADC14_configureConversionMemory(Y_AXIS_MEM, ADC_VREFPOS_AVCC_VREFNEG_VSS, Y_AXIS_INPUT, ADC_NONDIFFERENTIAL_INPUTS);

        ADC14_configureConversionMemory(X_AXIS_MEM, ADC_VREFPOS_AVCC_VREFNEG_VSS, X_AXIS_INPUT, ADC_NONDIFFERENTIAL_INPUTS);

        ADC14_configureConversionMemory(AIR_SENSOR_MEM,ADC_VREFPOS_AVCC_VREFNEG_VSS, AIR_SENSOR_INPUT, ADC_NONDIFFERENTIAL_INPUTS);

        ADC14_configureConversionMemory(TANK_SENSOR_MEM,ADC_VREFPOS_AVCC_VREFNEG_VSS, TANK_SENSOR_INPUT,ADC_NONDIFFERENTIAL_INPUTS);

        ADC14_configureConversionMemory(RESERVOIRE_SENSOR_MEM,ADC_VREFPOS_AVCC_VREFNEG_VSS,RESERVOIRE_SENSOR_INPUT,ADC_NONDIFFERENTIAL_INPUTS);


        //enabling interrupt when a conversion is complete
        ADC14_enableInterrupt(ADC_INT4);

        //Enable interrupts
        Interrupt_enableInterrupt(INT_ADC14);
        //Interrupt_enableMaster();


        ADC14_enableSampleTimer(ADC_AUTOMATIC_ITERATION);

        ADC14_enableConversion();
        ADC14_toggleConversionTrigger();

}

void ADC14_IRQHandler(void){
    uint64_t status = ADC14_getEnabledInterruptStatus();
    ADC14_clearInterruptFlag(status);
    if(status & ADC_INT4){
        //Air quality samples manually, no need for interrupt

        handle_joystick_interrupt(status);
        handle_water_level_interrupt(status);

    }
}
