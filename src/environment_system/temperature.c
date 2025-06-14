#include <environment_systems/temperature.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

#ifndef SOFTWARE_DEBUG
#include <ti/devices/msp432p4xx/driverlib/driverlib.h>
#include "msp.h"
#include "../../lib/HAL_I2C.h"
#endif

static TemperatureSensor ts = {.current_temperature = 21, .higher_threshold = 40, .lower_threshold = 20};

void temp_sensor_init() {
    #ifndef SOFTWARE_DEBUG
    I2C_setslave(TMP006_SLAVE_ADDRESS);
    I2C_write16(TMP006_WRITE_REG, TMP006_RST);

//    volatile int i;
//    for (i=10000; i>0;i--);

    I2C_write16(TMP006_WRITE_REG, TMP006_POWER_UP | TMP006_CR_2);

    BUZZER_PORT->DIR |= BUZZER_PIN;    // Set P2.7 as output
    BUZZER_PORT->OUT &= ~BUZZER_PIN;   // Initially off

    #else
    printf("TemperatureSensor and buzzer initialized\n");
    #endif

}

void temp_set_lower_threshold(uint8_t new_threshold){
    if(new_threshold < ts.higher_threshold){
        ts.lower_threshold = new_threshold;
        #ifdef SOFTWARE_DEBUG
        printf("Lower threshold set to %d\n", ts.lower_threshold);
        #endif
    }
}

uint8_t temp_get_lower_threshold() {
    return ts.lower_threshold;
}

void temp_set_higher_threshold(uint8_t new_threshold){
    if(new_threshold > ts.lower_threshold){
        ts.higher_threshold = new_threshold;
        #ifdef SOFTWARE_DEBUG
        printf("Higher threshold set to %d\n", ts.higher_threshold);
        #endif
    }
}

uint8_t temp_get_higher_threshold() {
    return ts.higher_threshold;
}

uint8_t temp_get_current_temperature() {
    return ts.current_temperature;
}

void temp_set_current_temperature(uint8_t temperature) {
    ts.current_temperature = temperature;
    #ifdef SOFTWARE_DEBUG
    printf("Current temperature set to %d \n", ts.current_temperature);
    #endif
}

int8_t would_goldilocks_like_this() {
    if(ts.current_temperature < ts.lower_threshold){
        #ifdef SOFTWARE_DEBUG
        printf("Temperature is too low\n");
        #endif
        return -1;
    }else if(ts.current_temperature > ts.higher_threshold){
        #ifdef SOFTWARE_DEBUG
        printf("Temperature is too high\n");
        #endif
        return 1;
    }else{
        #ifdef SOFTWARE_DEBUG
        printf("Temperature is just right\n");
        #endif
        return 0;
    }
}
void update_temperature(){

    uint8_t ambient_temp;
    #ifndef SOFTWARE_DEBUG
    I2C_setslave(TMP006_SLAVE_ADDRESS);
    ambient_temp = (uint8_t)((I2C_read16(TMP006_P_TABT) >> 2) * 0.03125);
    #else
    int temp_input;

    scanf("%d", &temp_input);

    if (temp_input >= 0 && temp_input <= 255) {
        ambient_temp = (uint8_t)temp_input;
    } else {
        printf("Input out of range for uint8_t!\n");
    }
    #endif

    temp_set_current_temperature(ambient_temp);

    int8_t comp = would_goldilocks_like_this();
    if(comp != 0){
        #ifndef SOFTWARE_DEBUG
        //activate buzzer
        BUZZER_PORT->OUT ^= BUZZER_PIN;

        #endif
        #ifdef DEBUG
            printf("Activating buzzer due to temperature out of range\n");
            printf("BUZZZZZZZZZZZZZZZZZZZZZZZZZZ\n");
        #endif

    }
    #ifndef SOFTWARE_DEBUG
    else if((BUZZER_PORT->OUT & BUZZER_PIN) != 0){

        BUZZER_PORT->OUT &= ~BUZZER_PIN; // deactivate buzzer
    }
    #endif
    #ifdef DEBUG
    else printf("Deactivating buzzer\n");
    #endif

    return;
}
