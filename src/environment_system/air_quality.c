#include "environment_systems/air_quality.h"
#include "environment_systems/temperature.h"
#include "environment_systems/buzzer_utils.h"
#include "scheduling/scheduler.h"

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>

#ifndef SOFTWARE_DEBUG
#include <ti/devices/msp432p4xx/driverlib/driverlib.h>
#endif

// Structure to hold air quality data
static Air air = {.threshold = 21, .current_level = 40, .stack_pos = 0};

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
                       update_air,  // Function to update air quality
                       2000,        // Task interval (to be updated)
                       2000,        // Time to process task (to be updated)
                       0            // Task status (inactive)
                       };
    // Adding task to scheduler and getting its stack position
    air.stack_pos = push_task(air_qual);

#ifdef DEBUG
    // Debug message for task addition
    printf("Added air quality task to stack\n");
#endif

    return;
}

void air_set_threshold(uint32_t level){
    air.threshold = level;

#ifdef DEBUG
    // Debug message to indicate threshold update
    printf("Air threshold set to %d\n", level);
#endif

    return;
}

void air_set_level(uint32_t level){
    air.current_level = level;

#ifdef DEBUG
    // Debug message to indicate level update
    printf("Air threshold set to %d\n", level);
#endif

    return;
}

uint32_t air_get_threshold(){
    return air.threshold;
}

uint32_t air_get_level(){
    return air.current_level;
}

bool exceeding_threshold(){

    // Check if current level exceeds threshold
    if(air.current_level >= air.threshold){

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

    bool exceeding = exceeding_threshold();

    if(exceeding){ // Turns buzzer on if threshold is exceeded

        // calling function to activate the buzzer
        turn_on_buzzer();

    } else {

        //calling function to deactivate buzzer
        turn_off_buzzer(would_goldilocks_like_this(), exceeding);

    }
    return;
}

void update_air_timer(int32_t new_timer){
    // setting the new timer value as specified by user
    task_list.task_array[air.stack_pos].max_time = new_timer;
    return;
}
