// updated 24/06 - succesfully added pwm to system. Done with the code, might need to change the tick period

#include "light_system/growing_light.h"
#include "scheduling/scheduler.h"
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

#ifndef SOFTWARE_DEBUG
#include "msp.h"
#include "ti/devices/msp432p4xx/driverlib/driverlib.h"
#include "../../lib/HAL_I2C.h"
#endif

// Initializing the grow light structure to hold the current state of the grow light system
GrowLight gl= {.current_brightness = 0, .threshold = MIN_BRIGHTNESS, .manual_mode = false, .on = false, .stack_pos=0};

// Timer_A Up Configuration Parameter
const Timer_A_UpModeConfig upConfig_led = {
            TIMER_A_CLOCKSOURCE_SMCLK,              // SMCLK = 3 MHz
            TIMER_A_CLOCKSOURCE_DIVIDER_1,          // SMCLK/32 = 1 kHz
            2500,                                  // 2500 ms tick period
            TIMER_A_TAIE_INTERRUPT_DISABLE,         // Disable Timer interrupt
            TIMER_A_CCIE_CCR0_INTERRUPT_DISABLE,    // Disable CCR0 interrupt
            TIMER_A_DO_CLEAR                        // Clear value
};
// CCR1 PWM configuration (initially off)
Timer_A_CompareModeConfig compareConfig_led = {
                TIMER_A_CAPTURECOMPARE_REGISTER_1,          // Use CCR1
                TIMER_A_CAPTURECOMPARE_INTERRUPT_ENABLE,    // Enable CCR interrupt
                TIMER_A_OUTPUTMODE_RESET_SET,               // Reset
                0                                           // 0% Duty Cycle initially
};
void grow_light_init(){
    // Setting up the I2C communication for the OPT3001 light sensor
    I2C_setslave(OPT3001_SLAVE_ADDRESS);
    // Reseting the OPT3001 sensor
    I2C_write16(CONFIG_REG, DEFAULT_CONFIG);

    // Configuring P7.7 to PM_TA1.1 for using PWM
    GPIO_setAsPeripheralModuleFunctionOutputPin(CCR1_PORT, CCR1_PIN, GPIO_PRIMARY_MODULE_FUNCTION);
    // Configuring P1.0 as GPIO output
    GPIO_setAsOutputPin(LED_PORT, LED_PIN);
    // Initialize Timer_A1 in Up mode
    Timer_A_configureUpMode(TIMER_A1_BASE, &upConfig_led);

    Timer_A_initCompare(TIMER_A1_BASE, &compareConfig_led);

    // Start Timer_A1
    Timer_A_startCounter(TIMER_A1_BASE, TIMER_A_UP_MODE);
    // Turning off the lights
    GPIO_setOutputLowOnPin(LED_PORT, LED_PIN);

#ifdef DEBUG
    // Debug message to indicate that the grow light has been initialized
    puts("GrowLight initialized\n");
#endif

    // Creating a task for updating the grow light system
        // The task will call the update_light function every 500 ms
        STask light =  {
            update_light,   // Function to update the grow light
            500,            // Task interval in milliseconds (to be updated)
            500,            // Time until task is processed in milliseconds (to be updated)
            0               // Task status, initially set to 0 (not active)
        };

        // Adding the task to the scheduler
        gl.stack_pos=push_task(light);
#ifdef DEBUG
    // Debug message to indicate that the grow light has been initialized
    puts("Added task to scheduler");
#endif
    return;
}

/********************************************
 *                  GETTERS                 *
 ********************************************/

uint32_t grow_light_get_brightness(){
    return gl.current_brightness;
}

uint32_t grow_light_get_threshold() {
    return gl.threshold;
}

bool is_grow_light_on() {
    return gl.on;
}

bool grow_light_get_mode() {
    return gl.manual_mode;
}

/********************************************
 *                  SETTERS                 *
 ********************************************/

void grow_light_set_brightness(uint32_t brightness){

    if(gl.current_brightness == brightness) return; // if the brightness is the same, do not set it
    gl.current_brightness = brightness; // set the new brightness value

#ifndef SOFTWARE_DEBUG
    // Update PWM duty cycle compare value to match brightness
    Timer_A_setCompareValue(TIMER_A1_BASE, TIMER_A_CAPTURECOMPARE_REGISTER_1, brightness);
#endif

#ifdef DEBUG
    // Debug message to indicate the brightness has changed
    printf("PWM Brightness set to %lu\n", brightness);
#endif

}

void grow_light_set_threshold(uint32_t new_threshold){

    if(gl.manual_mode){ // sets the threashold only if the grow light is in manual mode
        gl.threshold = new_threshold;

#ifdef DEBUG
        // Debug message to indicate the threshold has been set
        printf("Threshold set to %d\n", new_threshold);
#endif

    }
}

void grow_light_set_mode(int32_t manual_mode) {

    if(manual_mode == 0){ // turning off manual node

        // setting manual mode to false
        gl.manual_mode = false;
        Timer_A_startCounter(TIMER_A1_BASE, TIMER_A_UP_MODE);

    } else { // turning on manual mode

        // setting manual mode to true
        gl.manual_mode = true;
        // stopping timer
        Timer_A_stopTimer(TIMER_A1_BASE);
        Timer_A_clearInterruptFlag(TIMER_A1_BASE); // Optional

    }
    return;
}

void power_on_or_off(int32_t on){

    if (!gl.manual_mode) return; // Do not allow changing the leds in automatic mode

    if(on == 0){ // to turn on
        gl.on = false;
        GPIO_setOutputLowOnPin(LED_PORT, LED_PIN); // turn off leds manually
    }else{
        gl.on = true;
        GPIO_setOutputHighOnPin(LED_PORT, LED_PIN); // turn on leds manually
    }

    #ifdef DEBUG
        // Debug message to indicate the light has turned on or off manually
      printf("Manual light turned %s\n", gl.on ? "on" : "off");
    #endif
    return;
}
#ifndef SOFTWARE_DEBUG
void update_light(){

    if(!gl.manual_mode){ // If the grow light is not in manual mode, read the sensor value from the OPT3001 light sensor

        uint32_t exponent = 0;
        uint32_t sensor_val = 0;
        int32_t raw;

        // setting the I2C slave address for communication with the OPT3001 sensor
        I2C_setslave(OPT3001_SLAVE_ADDRESS);
        // reading the raw sensor value from the RESULT_REG register of the OPT3001 sensor
        raw = I2C_read16(RESULT_REG);

        // processing the raw sensor value to extract the sensor value and exponent
        sensor_val = raw & 0x0FFF;
        exponent = (raw>>12) & 0x000F;

        // Adjusting the sensor value based on the exponent, which represents the scaling factor
        switch (exponent) {
            case 0: sensor_val = sensor_val >> 6; break; // / 64
            case 1: sensor_val = sensor_val >> 5; break; // / 32
            case 2: sensor_val = sensor_val >> 4; break; // / 16
            case 3: sensor_val = sensor_val >> 3; break; // / 8
            case 4: sensor_val = sensor_val >> 2; break; // / 4
            case 5: sensor_val = sensor_val >> 1; break; // / 2
            case 6: sensor_val = sensor_val; break;      // * 1
            case 7: sensor_val = sensor_val << 1; break; // * 2
            case 8: sensor_val = sensor_val << 2; break; // * 4
            case 9: sensor_val = sensor_val << 3; break; // * 8
            case 10: sensor_val = sensor_val << 4; break; // * 16
            case 11: sensor_val = sensor_val << 5; break; // * 32
        }

#ifdef DEBUG
        // Debug message to indicate the sensor value that has been read and calculated
        puts("Sensor value: %d\n", sensor_val);
#endif
        uint32_t calculated_brightness;
        // If the sensor value is below the threshold, turn on the grow light and update the light intensity
        if (sensor_val >= gl.threshold) {
            calculated_brightness = 0;
            if(gl.on){
                gl.on = false;
                Timer_A_stopTimer(TIMER_A1_BASE);
                GPIO_setOutputLowOnPin(LED_PORT, LED_PIN);
            }
        } else {
            calculated_brightness = ((gl.threshold - sensor_val) * MAX_BRIGHTNESS) / gl.threshold; // [0,2500]
            if(!gl.on){
                gl.on = true;
                Timer_A_startCounter(TIMER_A1_BASE, TIMER_A_UP_MODE);
            }
        }
        grow_light_set_brightness(calculated_brightness);
    }
    return;
}
#else
// hardware independent code. does the same thing as update_light, but takes the raw value as a parameter
void update_light_hal(uint32_t raw){
    if(!grow_light_get_mode()){
        uint32_t exponent = 0;
        uint32_t sensor_val = 0;

        sensor_val = raw & 0x0FFF;
        exponent = (raw>>12) & 0x000F;

        switch (exponent) {
            case 0: sensor_val = sensor_val >> 6; break; // / 64
            case 1: sensor_val = sensor_val >> 5; break; // / 32
            case 2: sensor_val = sensor_val >> 4; break; // / 16
            case 3: sensor_val = sensor_val >> 3; break; // / 8
            case 4: sensor_val = sensor_val >> 2; break; // / 4
            case 5: sensor_val = sensor_val >> 1; break; // / 2
            case 6: sensor_val = sensor_val; break;      // * 1
            case 7: sensor_val = sensor_val << 1; break; // * 2
            case 8: sensor_val = sensor_val << 2; break; // * 4
            case 9: sensor_val = sensor_val << 3; break; // * 8
            case 10: sensor_val = sensor_val << 4; break; // * 16
            case 11: sensor_val = sensor_val << 5; break; // * 32
        }

        puts("Sensor value: %d\n", sensor_val);

        uint32_t calculated_brightness;
                // If the sensor value is below the threshold, turn on the grow light and update the light intensity
                if (sensor_val >= gl.threshold) {
                    calculated_brightness = 0;
                    if(gl.on){
                        gl.on = false;
                    }
                } else {
                    calculated_brightness = ((gl.threshold - sensor_val) * MAX_BRIGHTNESS) / gl.threshold; // [0,2500]
                    if(!gl.on){
                        gl.on = true;
                    }
                }
                grow_light_set_brightness(calculated_brightness);
    }
    return;
}
#endif

void TA1_N_IRQHandler(void){
    // Timer_A1 resigter 1 interrupt handler
    if (Timer_A_getCaptureCompareInterruptStatus(TIMER_A1_BASE,
                                                 TIMER_A_CAPTURECOMPARE_REGISTER_1,
                                                 TIMER_A_CAPTURECOMPARE_INTERRUPT_FLAG)){
        Timer_A_clearCaptureCompareInterrupt(TIMER_A1_BASE,
                                             TIMER_A_CAPTURECOMPARE_REGISTER_1);
        GPIO_toggleOutputOnPin(LED_PORT, LED_PIN);
    }
}

void update_light_timer(int32_t new_timer){
    // setting the new timer value as specified by user
    task_list.task_array[gl.stack_pos].max_time = new_timer;
    return;
}
