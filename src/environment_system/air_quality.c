#include "environment_systems/air_quality.h"
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>

#ifndef SOFTWARE_DEBUG
#include <ti/devices/msp432p4xx/driverlib/driverlib.h>
#endif

static Air air = {.threshold = 21, .current_level = 40};

void air_init(){
    #ifndef SOFTWARE_DEBUG
    GPIO_setAsPeripheralModuleFunctionInputPin(GPIO_PORT_P4,
                                               GPIO_PIN7,
                                               GPIO_TERTIARY_MODULE_FUNCTION);

    #else
    printf("Air initialized\n");
    #endif

}
void air_set_threshold(uint32_t level){
    air.threshold = level;
    #ifdef DEBUG
    printf("Air threshold set to %d\n", level);
    #endif
}

void air_set_level(uint32_t level){
    air.current_level = level;
    #ifdef DEBUG
    printf("Air threshold set to %d\n", level);
    #endif
}

uint32_t air_get_threshold(){
    return air.threshold;
}

uint32_t air_get_level(){
    return air.current_level;
}
bool exceeding_threshold(){
    if(air.current_level >= air.threshold){
        #ifdef DEBUG
        printf("Air level %d exceeds threshold %d\n", air.current_level, air.threshold);
        #endif
        return true;
    }else{
        #ifdef DEBUG
        printf("Air level %d is within threshold %d\n", air.current_level, air.threshold);
        #endif
        return false;
    }
}

void air_update(){

}
