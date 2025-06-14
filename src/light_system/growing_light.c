#include <light_system/growing_light.h>
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

#ifndef SOFTWARE_DEBUG
#include "msp.h"
#include "ti/devices/msp432p4xx/driverlib/driverlib.h"
#include "../../lib/HAL_I2C.h"
#endif

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

// initialization
void grow_light_init() {
    #ifndef SOFTWARE_DEBUG
    GPIO_setAsPeripheralModuleFunctionOutputPin(GPIO_PORT_P5, GPIO_PIN6,
                                               GPIO_PRIMARY_MODULE_FUNCTION);

    Timer_A_configureUpMode(TIMER_A2_BASE, &upConfig);
    Timer_A_startCounter(TIMER_A2_BASE, TIMER_A_UP_MODE);
//    P5->DIR |= BIT6;
    P5->OUT &= ~BIT6;

    I2C_setslave(OPT3001_SLAVE_ADDRESS);
    I2C_write16(CONFIG_REG, DEFAULT_CONFIG_100);

    compareConfig_PWM.compareRegister = TIMER_A_CAPTURECOMPARE_REGISTER_1;
    compareConfig_PWM.compareValue = gl.threshold;
    Timer_A_initCompare(TIMER_A2_BASE, &compareConfig_PWM);
    #else
    printf("GrowLight initialized\n");
    #endif
}
//getters

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

// setters
void grow_light_set_brightness(uint32_t brightness) {

    if(brightness > MAX_BRIGHTNESS){
        gl.current_brightness = MAX_BRIGHTNESS;
    } else if (brightness < MIN_BRIGHTNESS) {
        gl.current_brightness = MIN_BRIGHTNESS;
    } else {
        gl.current_brightness = brightness;
    }
    #ifdef SOFTWARE_DEBUG
    printf("Brightness set to %d\n", gl.current_brightness);
    #endif

}

void grow_light_set_threshold(uint32_t new_threshold){
    if(!gl.manual_mode){
        gl.threshold = new_threshold;
        #ifdef SOFTWARE_DEBUG
        printf("Threshold set to %d\n", new_threshold);
        #endif
    }
}



void grow_light_set_mode(bool manual_mode) {
    gl.manual_mode = manual_mode;
    #ifdef SOFTWARE_DEBUG
    if(manual_mode) {
        printf("Manual mode set\n");
    } else {
        printf("Automatic mode set\n");
    }
    #endif
}

void power_on_or_off(){
    #ifndef SOFTWARE_DEBUG
    P5->DIR |= BIT6;
    #endif
    if(gl.on) {
        gl.on = false;
        #ifndef SOFTWARE_DEBUG
        P5->OUT &= ~BIT6;
        #else
        printf("Grow light turned off\n");
        #endif
    } else {
        gl.on = true;
        #ifndef SOFTWARE_DEBUG
        P5->OUT |= BIT6;
        #else
        printf("Grow light turned on\n");
        #endif
    }
}

void update_light_intensity(uint32_t sensor_val){
    if(gl.on){
        uint32_t calculated_brightness = ((gl.threshold - sensor_val) * MAX_BRIGHTNESS) / gl.threshold;
        grow_light_set_brightness(calculated_brightness);
    }
}

// hardware dependent code
#ifndef SOFTWARE_DEBUG
void update_light(){
    if(!gl.manual_mode){
        uint32_t exponent = 0;
        uint32_t sensor_val = 0;
        int32_t raw;

        I2C_setslave(OPT3001_SLAVE_ADDRESS);
        raw = I2C_read16(RESULT_REG);

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

        #ifdef DEBUG
        printf("Sensor value: %d\n", sensor_val);
        #endif

        if(sensor_val < gl.threshold){
            if(!gl.on) {

                power_on_or_off();
            }
            update_light_intensity(sensor_val);
        } else if(sensor_val >= gl.threshold) {
            if(gl.on) {
                power_on_or_off();
                update_light_intensity(0);
            }

        }
    }
}
#else
// hardware independent code
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
                update_light_intensity(0);
            }
        }
    }
}
#endif
