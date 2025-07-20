/*
 * ===============================================================================
 * ADC (ANALOG-TO-DIGITAL CONVERTER) SYSTEM
 * ===============================================================================
 * 
 * This module handles all analog sensor readings for the greenhouse system.
 * The ADC converts continuous analog voltages into digital values that the
 * microcontroller can process and use for decision-making.
 * 
 * SENSORS CONNECTED TO ADC:
 * - Joystick (X and Y axes) - For user interface navigation
 * - Air Quality Sensor (MQ135) - Measures air pollution/CO2 levels
 * - Water Tank Level Sensor - Monitors main water tank level
 * - Water Reservoir Level Sensor - Monitors backup water reservoir
 * 
 * ADC CONFIGURATION:
 * - 14-bit resolution (0-16383 digital values)
 * - Multiple channel scanning (reads all sensors in sequence)
 * - Interrupt-driven operation for efficiency
 * - Automatic sampling with configurable timing
 * 
 * The ADC operates in multi-sequence mode, automatically cycling through
 * all configured channels and generating interrupts when conversions complete.
 */

// INCLUDES
#include "adc/adc.h"
#include "stdint.h"
#include <ti/devices/msp432p4xx/driverlib/driverlib.h>

// RELATED SYSTEM MODULES
#include "option_menu/option_menu_input.h"
#include "water_management/water_reading.h"
#include "environment_systems/air_quality.h"

/*
 * ADC INITIALIZATION FUNCTION
 */

void adc_init() {
    
    // STEP 1: GPIO PIN CONFIGURATION FOR ANALOG INPUTS
    // Each analog sensor needs its GPIO pin configured for analog input mode
    // instead of digital I/O mode. The pins are set to "tertiary module function"
    // which connects them directly to the ADC hardware.

    
    // Configure Pin P4.4 for Y-axis (vertical) joystick movement
    GPIO_setAsPeripheralModuleFunctionInputPin(Y_AXIS_PORT,    // Port P4
                                              Y_AXIS_PIN,      // Pin 4
                                              GPIO_TERTIARY_MODULE_FUNCTION); // Analog mode
    
    // Configure Pin P6.0 for X-axis (horizontal) joystick movement  
    GPIO_setAsPeripheralModuleFunctionInputPin(X_AXIS_PORT,    // Port P6
                                              X_AXIS_PIN,      // Pin 0
                                              GPIO_TERTIARY_MODULE_FUNCTION); // Analog mode

    GPIO_setAsPeripheralModuleFunctionInputPin(AIR_SENSOR_PORT,  // Port P4
                                              AIR_SENSOR_PIN,    // Pin 3
                                              GPIO_TERTIARY_MODULE_FUNCTION); // Analog mode
    
    // Main water tank level sensor
    GPIO_setAsPeripheralModuleFunctionInputPin(TANK_SENSOR_PORT,    // Port P5
                                              TANK_SENSOR_PIN,      // Pin 4
                                              GPIO_TERTIARY_MODULE_FUNCTION); // Analog mode
    
    // Backup water reservoir level sensor
    GPIO_setAsPeripheralModuleFunctionInputPin(RESERVOIRE_SENSOR_PORT, // Port P5
                                              RESERVOIRE_SENSOR_PIN,    // Pin 5
                                              GPIO_TERTIARY_MODULE_FUNCTION); // Analog mode
    // STEP 2: ADC MODULE CONFIGURATION
    // Configure the ADC hardware for optimal performance and power efficiency
    
    // Enable the ADC14 module (this powers up the ADC hardware)
    ADC14_enableModule();
    
    // Initialize the ADC with clock and timing settings
    // Clock configuration affects conversion speed and accuracy:
    // - ADCOSC: Use internal ADC oscillator (independent of system clock)
    // - PREDIVIDER_64: Divide input clock by 64 for stability
    // - DIVIDER_8: Further divide by 8 for optimal conversion timing
    // - 0: No additional flags
    // Total division: ADC clock = ADCOSC / (64 * 8) = ADCOSC / 512
    ADC14_initModule(ADC_CLOCKSOURCE_ADCOSC,
                    ADC_PREDIVIDER_64,
                    ADC_DIVIDER_8,
                    0);

    // STEP 3: MULTI-SEQUENCE MODE CONFIGURATION
    // Set up the ADC to automatically scan through multiple channels
    // This allows us to read all sensors without manual intervention
    
    // Configure multi-sequence mode to scan from memory location 0 to 4
    // - ADC_MEM0 to ADC_MEM4: 5 memory locations for our 5 sensors
    // - true: Enable repeat mode (automatically restart sequence after completion)
    ADC14_configureMultiSequenceMode(ADC_MEM0,
                                     ADC_MEM4,
                                     true);

    // STEP 4: INDIVIDUAL SENSOR MEMORY CONFIGURATION
    // Each sensor gets its own memory location and configuration
    // All sensors use the same voltage reference (AVCC to VSS) for consistency
    
    ADC14_configureConversionMemory(Y_AXIS_MEM,
                                   ADC_VREFPOS_AVCC_VREFNEG_VSS,
                                   Y_AXIS_INPUT,
                                   ADC_NONDIFFERENTIAL_INPUTS);

    ADC14_configureConversionMemory(X_AXIS_MEM,
                                   ADC_VREFPOS_AVCC_VREFNEG_VSS,
                                   X_AXIS_INPUT,
                                   ADC_NONDIFFERENTIAL_INPUTS);

    ADC14_configureConversionMemory(AIR_SENSOR_MEM,
                                   ADC_VREFPOS_AVCC_VREFNEG_VSS,
                                   AIR_SENSOR_INPUT,
                                   ADC_NONDIFFERENTIAL_INPUTS);

    ADC14_configureConversionMemory(TANK_SENSOR_MEM,
                                   ADC_VREFPOS_AVCC_VREFNEG_VSS,
                                   TANK_SENSOR_INPUT,
                                   ADC_NONDIFFERENTIAL_INPUTS);

    
    ADC14_configureConversionMemory(RESERVOIRE_SENSOR_MEM,
                                   ADC_VREFPOS_AVCC_VREFNEG_VSS,
                                   RESERVOIRE_SENSOR_INPUT,
                                   ADC_NONDIFFERENTIAL_INPUTS);

    // STEP 5: INTERRUPT CONFIGURATION
    // Set up interrupts so the system can automatically process new ADC readings
    
    // Enable interrupt for ADC memory location 4 (the last sensor in sequence)
    // Since we're in multi-sequence mode, this interrupt fires when ALL sensors
    // have been read and the complete cycle is finished
    ADC14_enableInterrupt(ADC_INT4);     // Interrupt when MEM4 conversion completes

    // Enable the ADC14 interrupt in the NVIC (Nested Vector Interrupt Controller), which allows the processor to respond to ADC completion events
    Interrupt_enableInterrupt(INT_ADC14);

    // STEP 6: START AUTOMATIC CONVERSION PROCESS
    // Configure and start the continuous conversion process
    
    // Enable automatic sample timing - ADC will automatically trigger conversions
    // at regular intervals without software intervention
    ADC14_enableSampleTimer(ADC_AUTOMATIC_ITERATION);

    // Enable the conversion process (allows ADC to start converting)
    ADC14_enableConversion();

    // Trigger the first conversion cycle. 
    // After this, the ADC will automatically repeat the sequence continuously
    ADC14_toggleConversionTrigger();
}

void ADC14_IRQHandler(void) {
    
    // STEP 1: DETERMINE INTERRUPT SOURCE
    // Read which ADC interrupt(s) are currently active
    // The status variable will have bits set for each active interrupt
    uint64_t status = ADC14_getEnabledInterruptStatus();
    
    // STEP 2: CLEAR INTERRUPT FLAGS
    // Clear all interrupt flags to acknowledge we've handled them
    // This prevents the interrupt from firing again immediately
    // and allows the ADC to continue normal operation
    ADC14_clearInterruptFlag(status);
    
    // STEP 3: PROCESS THE CONVERSION COMPLETION
    // Check if this interrupt was caused by completion of the 4th memory location
    // (which indicates all sensors in our sequence have been read)
    if (status & ADC_INT4) {
        
        /*
         * SENSOR DATA PROCESSING DISPATCH
         * 
         * At this point, all 5 ADC memory locations contain fresh sensor readings.
         * We delegate processing to specialized functions in other modules
         * that know how to interpret and act on each type of sensor data.
         */
        
        // JOYSTICK INPUT PROCESSING
        handle_joystick_interrupt(status);
        
        // WATER LEVEL MONITORING
        handle_water_level_interrupt(status);
        
        /*
         * NOTE: AIR QUALITY PROCESSING
         * The air quality sensor (ADC_MEM2) is handled differently.
         * Instead of processing in this interrupt, the air quality module
         * manually reads the ADC value when needed.
         */
    }
    
    // STEP 4: RETURN FROM INTERRUPT
    // Function automatically returns to the interrupted program
    // The ADC will continue its automatic conversion cycle
    // and this interrupt will fire again when the next cycle completes
}
