#include "environment_systems/temperature.h"
#include "environment_systems/air_quality.h"
#include "environment_systems/buzzer.h"

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#ifndef SOFTWARE_DEBUG
#include "scheduling/scheduler.h"
#include "IOT/IOT_communication.h"

#include "msp.h"
#include "../lib/HAL_I2C.h"

// Initialization of the temperature sensor to hold temperature sensor data
static TemperatureSensor ts = {.current_temperature = 21, .higher_threshold = 30, .lower_threshold = 20, .stack_pos=0};

#else

static TemperatureSensor ts = {.current_temperature = 21, .higher_threshold = 30, .lower_threshold = 20};

#endif

void temp_sensor_init() {
#ifndef SOFTWARE_DEBUG

    // seting up I2C communication for the TMP006 temperature sensor to get temperature data
    I2C_setslave(TMP006_SLAVE_ADDRESS);
    // Reseting the TMP006 sensor
    I2C_write16(TMP006_WRITE_REG, TMP006_RST);

    // Power up the TMP006 sensor and set the conversion rate to 2Hz
    // TMP006_POWER_UP is a bit to power up the sensor, TMP006_CR_2 is the conversion rate
    I2C_write16(TMP006_WRITE_REG, TMP006_POWER_UP | TMP006_CR_2);

#ifdef DEBUG
    // Debug message to indicate that the temperature sensor and buzzer have been initialized
    puts("Temperature Sensor initialized\n");
#endif

    // Creating a task for updating temperature sensor data
    // The task will call the update_temperature function every 5000 ms
    STask temp =  {
        update_temperature,  // Function to update temperature
        5500,                // Task interval in milliseconds (5 seconds)
        5500,                // Time to process task in milliseconds (5 seconds)
        true                 // Task status, initially set to true (active)
    };

    // Adding the temperature update task to the scheduler to ensure it runs periodically
    ts.stack_pos = push_task(temp);

#ifdef DEBUG
    // Debug message to indicate that the temperature sensor task has been added to the scheduler
    puts("Added temperature sensor to stack\n");
#endif
#endif
    return;
}

void temp_set_lower_threshold(uint8_t new_threshold){
    if(new_threshold < ts.higher_threshold){ // Ensure the new threshold is less than the higher threshold
        // Update the lower threshold
        ts.lower_threshold = new_threshold;

        #ifdef DEBUG
        // Debug message to indicate the lower threshold has been set
        printf("Lower threshold set to %d\n", ts.lower_threshold);
        #endif
    }
}

uint8_t temp_get_lower_threshold() {
    return ts.lower_threshold;
}

void temp_set_higher_threshold(uint8_t new_threshold){

    if(new_threshold > ts.lower_threshold){ // Ensure the new threshold is greater than the lower threshold

        // Update the higher threshold
        ts.higher_threshold = new_threshold;

        #ifdef DEBUG
        // Debug message to indicate the higher threshold has been set
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

   #ifdef DEBUG
    // Debug message to indicate the new temperature has been set
    printf("Current temperature set to %d \n", ts.current_temperature);
   #endif

}

int8_t would_goldilocks_like_this() {
    if(ts.current_temperature < ts.lower_threshold){

        #ifdef DEBUG
        // Debug message to indicate the temperature is too low
        puts("Temperature is too low\n");
        #endif
#ifndef SOFTWARE_DEBUG
        send_data(2, 0, 1);
#endif
        return -1;
    }else if(ts.current_temperature > ts.higher_threshold){

        #ifdef DEBUG
        // Debug message to indicate the temperature is too high
        puts("Temperature is too high\n");
        #endif
#ifndef SOFTWARE_DEBUG
        send_data(2, 0, 3);
#endif
        return 1;
    }else{
        #ifdef DEBUG
            // Debug message to indicate the temperature is within the acceptable range
            puts("Temperature is just right\n");
        #endif
#ifndef SOFTWARE_DEBUG
            send_data(2, 0, 2);
#endif
        return 0;
    }
}

void update_temperature(){

    send_data(2, 1, 0);

    uint8_t ambient_temp;
    #ifndef SOFTWARE_DEBUG

    // Setting the I2C slave address for communication from the TMP006 sensor
    I2C_setslave(TMP006_SLAVE_ADDRESS);

    // The temperature is read from the 16 TMP006_P_TABT register, shifted right by 2 bits to get the actual temperature value
    // and then multiplied by 0.03125 to convert it to degrees Celsius
    ambient_temp = (uint8_t)((I2C_read16(TMP006_P_TABT) >> 2) * 0.03125);

    #else

    // For software debugging, we simulate the ambient temperature input
    puts("Enter ambient temperature (0-255): ");
    int temp_input;

    // Read the ambient temperature input from the user
    scanf("%d", &temp_input);

    // Check if the input is within the range of uint8_t (0-255)
    if (temp_input >= 0 && temp_input <= 255) {
        ambient_temp = (uint8_t)temp_input;
    } else {
        puts("Input out of range for uint8_t!\n");
    }

    #endif

#ifdef DEBUG
    printf("TEMPERATURE: %u\n", ambient_temp);
#endif

    // Setting to the current temperature
    temp_set_current_temperature(ambient_temp);

    // Chceking if the current temperature is within the acceptable range
    int8_t comp = would_goldilocks_like_this();

    // Active buzzer if the temperature is outside the acceptable range
    if(comp != 0){
        // Calling function to activate the buzzer if in automatic mode
        if(!get_buzzer_manual_mode()){
            turn_on_buzzer();
        }
    } else {
        // Calling function to deactivate buzzer if in automatic mode
        if(!get_buzzer_manual_mode()){
            turn_off_buzzer(comp, exceeding_threshold());
        }
    }
    return;
}

#ifndef SOFTWARE_DEBUG
void update_temperature_timer(int32_t new_timer){
    // Setting the new timer value as specified by user
    task_list.task_array[ts.stack_pos].max_time = new_timer;
    return;
}
#endif
