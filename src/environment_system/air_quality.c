#include "environment_systems/air_quality.h"
#include "scheduling/scheduler.h"
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>

#ifndef SOFTWARE_DEBUG
#include <ti/devices/msp432p4xx/driverlib/driverlib.h>
#endif

// Structure to hold air quality data
static Air air = {.threshold = 21, .current_level = 40};

void air_init(){
#ifndef SOFTWARE_DEBUG
    // Configuring GPIO pin for air quality sensor
    GPIO_setAsPeripheralModuleFunctionInputPin(GPIO_PORT_P4,
                                               GPIO_PIN7,
                                               GPIO_TERTIARY_MODULE_FUNCTION);

#endif
#ifdef DEBUG
    // Debug message to ensure the gpio pin is set up
    printf("Air initialized\n");
#endif

    // Creating a task for updating air quality
    STask air_qual =  {
                       update_air, // Function to update air quality
                       1000, // Task interval (to be updated)
                       1000, // Time to process task (to be updated)
                       0 // Task status (inactive)
                       };
    // Adding task to scheduler
    push_task(air_qual);

#ifdef DEBUG
    // Debug message for task addition
    printf("Added air quality task to stack\n");
#endif

}

void air_set_threshold(uint32_t level){
    air.threshold = level;
#ifdef DEBUG
    // Debug message to indicate threshold update
    printf("Air threshold set to %d\n", level);
#endif
}

void air_set_level(uint32_t level){
    air.current_level = level;
#ifdef DEBUG
    // Debug message to indicate level update
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
    if(air.current_level >= air.threshold){  // Check if current level exceeds threshold
#ifdef DEBUG
        // Debug message to indicate threshold has been exceeded
        printf("Air level %d exceeds threshold %d\n", air.current_level, air.threshold);
#endif
        return true;
    }else{ // If current level is below threshold
#ifdef DEBUG
        // Debug message to indicate air quality is within threshold
        printf("Air level %d is within threshold %d\n", air.current_level, air.threshold);
#endif
        return false;
    }
}

void update_air(){
    uint32_t level = 0; // Placeholder for sensor value from pin 4.7
    air_set_level(level); // Update air quality level
    if(exceeding_threshold()){ // Turns buzzer on if threshold is exceeded

        // ...
        
#ifdef DEBUG
        print("Turning buzzer on for air quality\n");
        print("BUZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZ\n");
#endif
    }else if(1){ // Check if the buzzer is on // Turns buzzer off if within threshold

        // ...

#ifdef DEBUG
        print("Turning buzzer off for air quality\n");
#endif
    }
}
