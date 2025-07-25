/*
 * This file implements air quality monitoring using an MQ135 gas sensor
 * The MQ135 can detect various gases including CO2, NH3, NOx, alcohol, benzene, smoke, etc.
 */
#include "environment_systems/air_quality.h"
#include "environment_systems/temperature.h"
#include "environment_systems/buzzer.h"

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>

#ifndef SOFTWARE_DEBUG
#include "scheduling/scheduler.h"
#include "IOT/IOT_communication.h"
#include "adc/adc.h"
#include <ti/devices/msp432p4xx/driverlib/driverlib.h>
#endif

#ifndef SOFTWARE_DEBUG

// Global air quality data structure
static Air air = {
    .threshold = 500, // Default air quality threshold (ppm)
    .current_level = 0,
    .stack_pos = 0
};

#else

// Global air quality for testing purposes
static Air air = {
    .threshold = 500,
    .current_level = 0
};

#endif

void air_init(){
#ifdef DEBUG
    // Debug message to confirm initialization
    puts("Air initialized\n");
#endif

#ifndef SOFTWARE_DEBUG
    // Configure GPIO pin P4.3 as analog input for the MQ135 sensor to read the analog voltage output (uncomment the lines if using a msp with a working adc)
    // GPIO_setAsPeripheralModuleFunctionInputPin(GPIO_PORT_P4,
    //                                                GPIO_PIN3,
    //                                                GPIO_TERTIARY_MODULE_FUNCTION);

    // // Initialize and enable the ADC14 module for analog-to-digital conversion
    // ADC14_enableModule();
    // // Uses main clock (MCLK) with no prescaling for maximum resolution
    // ADC14_initModule(ADC_CLOCKSOURCE_MCLK, ADC_PREDIVIDER_1, ADC_DIVIDER_1, 0);

    // // Configure ADC memory slot 2 for single sample mode, to set up the storage of the conversion results
    // ADC14_configureSingleSampleMode(ADC_MEM2, true);

    // // Configuring the conversion parameters
    // ADC14_configureConversionMemory(ADC_MEM2,
    //     ADC_VREFPOS_AVCC_VREFNEG_VSS, // Uses AVCC (3.3V) as positive reference and VSS (0V) as negative reference
    //     ADC_INPUT_A10, // Read from analog input A10 (which corresponds to P4.3)
    //     false); // No differential input mode

    // // Enable sample timer in manual iteration mode, to trigger conversions manually when needed
    // ADC14_enableSampleTimer(ADC_MANUAL_ITERATION);

    // // Enable the ADC for conversions
    // ADC14_enableConversion();

    // Create a scheduled task for periodic air quality updates
    // This task will run the update_air() function every 10 seconds
    STask air_qual = {
                      update_air,   // Function pointer to the update function
                      11500,        // Task interval in milliseconds (11.5 seconds)
                      11500,        // Maximum time allowed for task execution
                      true          // Initial task status (active)
    };
    
    // Add the task to the scheduler and store its position for future reference
    air.stack_pos = push_task(air_qual);
#endif

#ifdef DEBUG
    // Debug confirmation that task was added successfully
    puts("Added air quality task to stack\n");
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
    printf("Air level set to %d\n", level);
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
    if(air.current_level > air.threshold){

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

#ifndef SOFTWARE_DEBUG
void update_air(){

    // Commented to prevent collisions with the interrupt handler
    // Trigger a new ADC conversion
//    ADC14_toggleConversionTrigger();
    // Wait for the ADC conversion to complete
//    while (ADC14_isBusy());

     // Read the 14-bit ADC result from memory slot 2
    uint32_t adcValue = ADC14_getResult(AIR_SENSOR_MEM);

    // Convert ADC value to voltage (0-3.3V range)
    // 16383 is the maximum 14-bit value (2^14 - 1)
    float adcVoltage = ((float) adcValue * VREF) / 16383.0f;
    // Calculate sensor resistance (Rs) using refernce and calculated voltages
    float Rs = RL * ((VCC / adcVoltage) - 1.0f);
    // Calculate the ratio of current resistance to baseline resistance
    float ratio = Rs / R0;
    // Convert resistance ratio to gas concentration using logarithmic formula
    float logppm = M * log10f(ratio) + B;
    // Convert from log scale to actual ppm value
    uint32_t level = (uint32_t) powf(10.0f, logppm);

    // Update the system air quality level with the calculated value
    air_set_level(level);

    // Check if the new reading exceeds the safety threshold
    bool exceeding = exceeding_threshold();

    if(exceeding){ // Turns buzzer on if threshold is exceeding
        // Sending data to IOT system to indicate air quality issue
        send_data(1,0,2);
        // Calling function to activate the buzzer if in automatic mode
        if(!get_buzzer_manual_mode()){
            turn_on_buzzer();
        }
        
    } else {
        // Sending data to IOT system to indicate air quality is normal
        send_data(1,0,1);
        // Calling function to deactivate buzzer if in automatic mode
        if(!get_buzzer_manual_mode()){
            turn_off_buzzer(would_goldilocks_like_this(), exceeding);
        }
    }
    return;
}

#else

void update_air_hal(uint32_t level){
    // Update the system air quality level with the calculated value
    air_set_level(level);

    // Check if the new reading exceeds the safety threshold
    bool exceeding = exceeding_threshold();

    if(exceeding){ // Turns buzzer on if threshold is exceeded and in automatic mode 
        // Calling function to activate the buzzer if in automatic mode
        if(!get_buzzer_manual_mode()){
            turn_on_buzzer();
        }
    } else {
        // Calling function to deactivate buzzer if in automatic mode
        if(!get_buzzer_manual_mode()){
            turn_off_buzzer(would_goldilocks_like_this(), exceeding);
        }
    }
    return;
}
#endif

#ifndef SOFTWARE_DEBUG
float calibrateR0(int32_t no_samples) {
    float sumRs = 0.0f;      // Sum of all valid resistance readings
    int valid_samples = 0;   // Count of valid samples (non-zero voltage)
    int i = 0;
    

    // Take multiple samples for averaging
    for (; i < no_samples; i++) {
        // Trigger ADC conversion and wait for completion
        ADC14_toggleConversionTrigger();
            while (ADC14_isBusy());
            uint32_t adcValue = ADC14_getResult(ADC_MEM2);

            // Convert ADC reading to voltage
            float Vout = (adcValue * 3.3f) / 16383.0f;

            // Skip samples with very low voltage (invalid readings)
            if(Vout < 0.01f) continue;
            ++valid_samples;

            // Calculate sensor resistance using voltage divider formula
            float Rs = RL * ((VCC / Vout) - 1.0f);
            sumRs += Rs;
    }
    printf("Sum: %f, samples: %d\n", sumRs, valid_samples);
    
    // Return the average resistance value
    return (sumRs / (float)valid_samples);
}

void update_air_timer(int32_t new_timer){
    // setting the new timer value as specified by user
    task_list.task_array[air.stack_pos].max_time = new_timer;
    return;
}
#endif
