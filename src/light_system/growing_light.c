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
GrowLight gl= {.current_brightness = 0, .threshold = MIN_BRIGHTNESS, .manual_mode = false, .on = false};

#ifndef SOFTWARE_DEBUG
Timer_A_CompareModeConfig compareConfig_PWM = {
TIMER_A_CAPTURECOMPARE_REGISTER_3,          // Use CCR3
        TIMER_A_CAPTURECOMPARE_INTERRUPT_DISABLE,   // Disable CCR interrupt
        TIMER_A_OUTPUTMODE_TOGGLE_SET,              // Toggle output but
        7500                                        // 50% Duty Cycle
        };
const Timer_A_UpModeConfig upConfig = {
TIMER_A_CLOCKSOURCE_SMCLK,                      // SMCLK = 3 MhZ
        TIMER_A_CLOCKSOURCE_DIVIDER_12,         // SMCLK/12 = 250 KhZ
        10000,                                  // 40 ms tick period
        TIMER_A_TAIE_INTERRUPT_DISABLE,         // Disable Timer interrupt
        TIMER_A_CCIE_CCR0_INTERRUPT_DISABLE,    // Disable CCR0 interrupt
        TIMER_A_DO_CLEAR                        // Clear value
        };
#endif


void grow_light_init(){
#ifndef SOFTWARE_DEBUG
    // Configuring GPIO pin for the grow light
    GPIO_setAsPeripheralModuleFunctionOutputPin(GPIO_PORT_P5, GPIO_PIN6,
                                               GPIO_PRIMARY_MODULE_FUNCTION);

    // Configuring Timer A2 for PWM output
    Timer_A_configureUpMode(TIMER_A2_BASE, &upConfig);
    // starting Timer A2
    Timer_A_startCounter(TIMER_A2_BASE, TIMER_A_UP_MODE);
    // setting direction to output
//    P5->DIR |= BIT6;
    // turning off the lights, in case the grow light is on 
    P5->OUT &= ~BIT6; 

    // Setting up the I2C communication for the OPT3001 light sensor
    I2C_setslave(OPT3001_SLAVE_ADDRESS);
    // Reseting the OPT3001 sensor
    I2C_write16(CONFIG_REG, DEFAULT_CONFIG_100);

    // Configuring the OPT3001 sensor to operate for a 100ms conversion time
    compareConfig_PWM.compareRegister = TIMER_A_CAPTURECOMPARE_REGISTER_1;
    // Setting the compare value to the current brightness level for the grow light (should be off initially)
    compareConfig_PWM.compareValue = gl.current_brightness;
    // Configuring the compare mode for PWM output
    Timer_A_initCompare(TIMER_A2_BASE, &compareConfig_PWM);
#endif
#ifdef DEBUG
    // Debug message to indicate that the grow light has been initialized
    printf("GrowLight initialized\n");
#endif

    // Creating a task for updating the grow light system
    // The task will call the update_light function every 500 ms
    STask light =  {
                       update_light, // Function to update the grow light
                       500, // Task interval in milliseconds (to be updated)
                       500, // Time until task is processed in milliseconds (to be updated)
                       0 // Task status, initially set to 0 (not active)
                    };
    // Adding the task to the scheduler
    push_task(light);
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

void grow_light_set_brightness(uint32_t brightness) {

    if(brightness > MAX_BRIGHTNESS){ // if brightness is greater than the maximum allowed brightness
        // set the brightness to the maximum allowed brightness
        gl.current_brightness = MAX_BRIGHTNESS;
    } else if (brightness < MIN_BRIGHTNESS) { // if brightness is less than the minimum allowed brightness
        // set the brightness to the minimum allowed brightness
        gl.current_brightness = MIN_BRIGHTNESS;
    } else {
        // set the brightness to the given value
        gl.current_brightness = brightness;
    }
#ifdef DEBUG
    // Debug message to indicate the brightness has been set
    printf("Brightness set to %d\n", gl.current_brightness);
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

void grow_light_set_mode(bool manual_mode) {
    gl.manual_mode = manual_mode;
    #ifdef DEBUG
    if(manual_mode) {
        // Debug message to indicate the grow light is set to manual mode
        printf("Manual mode set\n");
    } else {
        // Debug message to indicate the grow light is set to automatic mode
        printf("Automatic mode set\n");
    }
    #endif
}

void power_on_or_off(){ 
#ifndef SOFTWARE_DEBUG
    // Toggle the grow light power state by changing the output of the GPIO pin
    P5->DIR |= BIT6;
#endif
    if(gl.on) {
        gl.on = false;
#ifndef SOFTWARE_DEBUG
        // Turn off the grow light by clearing the output of the GPIO pin
        P5->OUT &= ~BIT6;
#endif
#ifdef DEBUG
        // Debug message to indicate the grow light is turned off
        printf("Grow light turned off\n");
#endif
    } else {
        gl.on = true;
#ifndef SOFTWARE_DEBUG
        // Turn on the grow light by setting the output of the GPIO pin
        P5->OUT |= BIT6;
#endif
#ifdef DEBUG
        // Debug message to indicate the grow light is turned on
        printf("Grow light turned on\n");
#endif
    }
}

//  need to figure out the value for when the light is off
void update_light_intensity(uint32_t sensor_val){
    uint32_t calculated_brightness;
    if(sensor_val == gl.threshold){
        calculated_brightness = 0;
    }else{
        calculated_brightness = ((gl.threshold - sensor_val) * MAX_BRIGHTNESS) / gl.threshold;
    }
    grow_light_set_brightness(calculated_brightness);
}

// hardware dependent code
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
        printf("Sensor value: %d\n", sensor_val);
#endif
        // If the sensor value is below the threshold, turn on the grow light and update the light intensity
        if(sensor_val < gl.threshold){
            if(!gl.on) {
                power_on_or_off(); // Turn on the grow light if it is not already on
            }
            update_light_intensity(sensor_val);
        } else { // If the sensor value is above the threshold, turn off the grow light and set the light intensity to 0
            if(gl.on) {
                power_on_or_off(); // Turn off the grow light if it is currently on
                update_light_intensity(gl.threshold); // Set the light intensity to 0 if the sensor value is above the threshold
            }

        }
    }
}

#ifdef SOFTWARE_DEBUG
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

        printf("Sensor value: %d\n", sensor_val);

        if(sensor_val < gl.threshold){
            if(!gl.on) {
                power_on_or_off();
            }
            update_light_intensity(sensor_val);
        }else if(sensor_val >= gl.threshold) {
            if(gl.on) {
                power_on_or_off();
                update_light_intensity(gl.threshold);
            }
        }
    }
}
#endif
