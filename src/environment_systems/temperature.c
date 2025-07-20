/*
 * TEMPERATURE MONITORING AND CONTROL SYSTEM
 * 
 * This module manages temperature monitoring for the plant environment.
 * It provides critical climate control by:
 * - Reading temperature from TMP006 infrared temperature sensor
 * - Comparing readings against configurable thresholds
 * - Triggering alerts when temperature goes outside acceptable range
 * - Integrating with buzzer system for audio alerts
 * - Sending temperature data to IoT systems for remote monitoring
 * 
 * GOLDILOCKS PRINCIPLE:
 * The system implements a "Goldilocks zone" concept where temperature
 * must be "just right" ie. not too hot, not too cold, for optimal plant growth.
 * 
 * INTEGRATION WITH OTHER SYSTEMS:
 * - Buzzer System: Provides audio alerts for temperature problems
 * - Air Quality System: Coordinates alerts (buzzer only turns off when ALL conditions are normal)
 * - IoT Communication: Sends temperature status to remote monitoring systems
 * - Scheduler System: Automatic periodic temperature checking
 */

#include "environment_systems/temperature.h"
#include "environment_systems/air_quality.h"
#include "environment_systems/buzzer.h"

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

// HARDWARE AND SYSTEM INCLUDES (only when using hardware)
#ifndef SOFTWARE_DEBUG
#include "scheduling/scheduler.h"
#include "IOT/IOT_communication.h"
#include "msp.h"
#include "../lib/HAL_I2C.h" // I2C communication library for sensor interface
#endif

#ifndef SOFTWARE_DEBUG
// Configuration when using real hardware (includes scheduler integration)
static TemperatureSensor ts = {
    .current_temperature = DEFAULT_TEMP,
    .higher_threshold = DEFAULT_HIGHER_THRESHOLD,
    .lower_threshold = DEFAULT_LOWER_THRESHOLD,
    .stack_pos = 0
};
#else
// Configuration for software simulation/testing (no scheduler needed)
static TemperatureSensor ts = {
    .current_temperature = DEFAULT_TEMP,
    .higher_threshold = DEFAULT_HIGHER_THRESHOLD,
    .lower_threshold = DEFAULT_LOWER_THRESHOLD
};
#endif

/*
 * TEMPERATURE SYSTEM INITIALIZATION
*/
void temp_sensor_init() {
    
#ifndef SOFTWARE_DEBUG
    // STEP 1: CONFIGURE I2C COMMUNICATION
    // Sets up communication with the TMP006 temperature sensor by using an I2C protocol for data exchange
    I2C_setslave(TMP006_SLAVE_ADDRESS);
    
    // STEP 2: RESET THE SENSOR
    // Send reset command to ensure sensor starts in a known state, which clears any previous configuration and ensures reliable operation
    I2C_write16(TMP006_WRITE_REG, TMP006_RST);
    
    // STEP 3: CONFIGURE SENSOR OPERATION
    // Power up the sensor and set conversion rate
    // TMP006_POWER_UP: Enables sensor operation (exits power-down mode)
    // TMP006_CR_2: Sets conversion rate to 2Hz (2 readings per second)
    I2C_write16(TMP006_WRITE_REG, TMP006_POWER_UP | TMP006_CR_2);

    // STEP 4: CREATE PERIODIC MONITORING TASK
    // Set up automatic temperature monitoring through the task scheduler, which ensures regular temperature checks without manual intervention
    
    STask temp_task = {
        .fpointer = update_temperature,    // Function to call for temperature updates
        .max_time = 5500,                 // Run every 5.5 seconds
        .elapsed_time = 5500,             // Initial delay before first execution
        .is_active = true                 // Task is enabled and will run
    };
    
    // STEP 5: INTEGRATE WITH SCHEDULER
    // Add our temperature task to the system scheduler and remember its position
    // The position is stored so we can modify the task timing later if needed
    ts.stack_pos = push_task(temp_task);

#ifdef DEBUG
    puts("Added temperature sensor to stack\n");
#endif
#endif
#ifdef DEBUG
    puts("Temperature Sensor initialized\n");
#endif
}

/*
 * TEMPERATURE THRESHOLD MANAGEMENT FUNCTIONS
 */

void temp_set_lower_threshold(uint8_t new_threshold){
    
    // Ensures new lower threshold doesn't exceed upper threshold, which prevents invalid temperature ranges
    if(new_threshold < ts.higher_threshold){ 
        
        // Stores the new minimum temperature threshold
        ts.lower_threshold = new_threshold;

        #ifdef DEBUG
        // Confirms threshold update for development/testing
        printf("Lower threshold set to %d\n", ts.lower_threshold);
        #endif
    }
    // NOTE: If validation fails, the threshold remains unchanged (safe default)
}

uint8_t temp_get_lower_threshold() {
    return ts.lower_threshold;
}

void temp_set_higher_threshold(uint8_t new_threshold){

    // Ensures new upper threshold exceeds lower threshold, which prevents invalid temperature ranges and maintains logical order
    if(new_threshold > ts.lower_threshold){ 

        // Stores the new maximum temperature threshold
        ts.higher_threshold = new_threshold;

        #ifdef DEBUG
        // Confirms threshold update for development/testing
        printf("Higher threshold set to %d\n", ts.higher_threshold);
        #endif

    }
    // If validation fails, the threshold remains unchanged (safe default)
}

uint8_t temp_get_higher_threshold() {
    return ts.higher_threshold;
}

uint8_t temp_get_current_temperature() {
    return ts.current_temperature;
}

void temp_set_current_temperature(uint8_t temperature) {

    // STORE: Update the global temperature state
    ts.current_temperature = temperature;

   #ifdef DEBUG
    // Logging temperature updates for development monitoring
    printf("Current temperature set to %d \n", ts.current_temperature);
   #endif

}

int8_t would_goldilocks_like_this() {
    
    // CONDITION 1: TOO COLD - Temperature below acceptable minimum
    // Plants may experience stunted growth, frost damage, or metabolic slowdown
    if(ts.current_temperature < ts.lower_threshold){

        #ifdef DEBUG
        // Logging cold temperature detection for monitoring
        puts("Temperature is too low\n");
        #endif
        
#ifndef SOFTWARE_DEBUG
        // Sends "too cold" status to monitoring system thorough IOT
        send_data(2, 0, 1);
#endif
        // -1 indicates temperature is below acceptable range
        return -1;
        
    // CONDITION 2: TOO HOT - Temperature above acceptable maximum  
    // Plants may experience heat stress, wilting, or cellular damage
    }else if(ts.current_temperature > ts.higher_threshold){

        #ifdef DEBUG
        // Logging hot temperature detection for monitoring
        puts("Temperature is too high\n");
        #endif
        
#ifndef SOFTWARE_DEBUG
        // Sends "too hot" status to monitoring system through IOT
        send_data(2, 0, 3);
#endif
        // +1 indicates temperature is above acceptable range
        return 1;
        
    // CONDITION 3: JUST RIGHT - Temperature within perfect range
    // Plants experience optimal growing conditions for photosynthesis and growth
    }else{
        
        #ifdef DEBUG
            // Log perfect temperature condition for monitoring
            puts("Temperature is just right\n");
        #endif
        
#ifndef SOFTWARE_DEBUG
            // Sends "perfect" status to monitoring system
            send_data(2, 0, 2);
#endif
            // 0 indicates temperature is within ideal range
            return 0;
    }
}

/*
 * CORE TEMPERATURE MONITORING AND CONTROL SYSTEM
*/
void update_temperature(){

    uint8_t ambient_temp;
    
    #ifndef SOFTWARE_DEBUG
    // HARDWARE MODE: REAL SENSOR READING
    
    // STEP 1: ESTABLISH I2C COMMUNICATION
    // Configure I2C to communicate with the TMP006 temperature sensor
    I2C_setslave(TMP006_SLAVE_ADDRESS);

    // STEP 2: READ AND CONVERT SENSOR DATA
    // Read temperature from the TMP006_P_TABT register (16-bit value)
    // 
    // DATA CONVERSION PROCESS:
    // 1. Read 16-bit raw value from sensor register
    // 2. Shift right by 2 bits to remove status bits (keeps 14-bit temp data)
    // 3. Multiply by 0.03125 to convert to degrees Celsius
    // 4. Cast to uint8_t for system compatibility
    ambient_temp = (uint8_t)((I2C_read16(TMP006_P_TABT) >> 2) * 0.03125);

    #else
    // SOFTWARE DEBUG MODE: For development and testing, allow manual temperature entry, without hardware dependency
    
    puts("Enter ambient temperature (0-255): ");
    int temp_input;

    // STEP 1: GET USER INPUT
    // Read temperature value from developer/tester
    scanf("%d", &temp_input);

    // STEP 2: VALIDATE INPUT RANGE
    // Ensure input fits within uint8_t range (0-255)
    // This prevents system crashes from invalid data
    if (temp_input >= 0 && temp_input <= 255) {
        ambient_temp = (uint8_t)temp_input;
    } else {
        puts("Input out of range for uint8_t!\n");
        return; // Exit function if invalid input provided
    }

    #endif

#ifdef DEBUG
    // Displaying current temperature reading for development monitoring
    printf("TEMPERATURE: %u\n", ambient_temp);
#endif

    // STEP 3: UPDATE SYSTEM TEMPERATURE STATE
    // Store the new temperature reading in the global system state, to make it available for other components
    temp_set_current_temperature(ambient_temp);

    // STEP 4: GOLDILOCKS EVALUATION
    int8_t comp = would_goldilocks_like_this();

    // STEP 5: AUTOMATED ALERT MANAGEMENT
    // Control buzzer alerts based on temperature status and user preferences
    
    // CONDITION A: TEMPERATURE OUT OF RANGE
    // If temperature is too hot OR too cold
    if(comp != 0){
        // Activate buzzer ONLY if not in manual mode
        // This respects user preference to disable automatic alerts
        if(!get_buzzer_manual_mode()){
            turn_on_buzzer();
        }
        
    // CONDITION B: TEMPERATURE IN PERFECT RANGE  
    // If temperature is "just right" (comp == 0)
    } else {
        // Deactivate buzzer ONLY if not in manual mode
        // This prevents automatic buzzer control when user wants manual control
        if(!get_buzzer_manual_mode()){
            // exceeding_threshold() provides additional context for buzzer control
            turn_off_buzzer(comp, exceeding_threshold());
        }
    }
}

#ifndef SOFTWARE_DEBUG
void update_temperature_timer(int32_t new_timer){
    
    // DIRECT SCHEDULER UPDATE: Modify the task timing in the scheduler
    // Access the specific temperature task in the scheduler's task array
    // using the stored position (ts.stack_pos) and update its timing
    task_list.task_array[ts.stack_pos].max_time = new_timer;
}
#endif
