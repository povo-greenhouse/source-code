#include "light_system/growing_light.h"

// Standard C library includes for basic functionality
#include <stdio.h>    // For printf() and other input/output functions
#include <stdint.h>   // For fixed-width integer types like uint32_t
#include <stdbool.h>  // For boolean type

// Conditional compilation: only include these when testing without actual hardware
#ifndef SOFTWARE_DEBUG
#include "scheduling/scheduler.h"      // For task scheduling functionality
#include "IOT/IOT_communication.h"    // For sending data to IoT systems

// Hardware-specific includes for MSP432 microcontroller
#include "msp.h"                                           // MSP432 basic definitions
#include "ti/devices/msp432p4xx/driverlib/driverlib.h"     // TI driver library
#include "../lib/HAL_I2C.h"                                // I2C communication library
#endif

// Global variable: Our grow light system state, which holds all the important information
// When we use hardware, we also track the scheduler position
#ifndef SOFTWARE_DEBUG
GrowLight gl = {
    .current_brightness = 0,           // Start with lights off (0% brightness)
    .threshold = DEFAULT_THRESHOLD,    // Light level that triggers our lights
    .manual_mode = false,              // Start in automatic mode
    .on = false,                       // Lights are initially off
    .stack_pos = 0                     // Position in the task scheduler
};
#else
// When in software debug mode, we don't need scheduler tracking
GrowLight gl = {
    .current_brightness = 0,           // Start with lights off
    .threshold = DEFAULT_THRESHOLD,    // Light level threshold
    .manual_mode = false,              // Start in automatic mode
    .on = false                        // Lights are initially off
};
#endif

// Only compiles when we're using real hardware
#ifndef SOFTWARE_DEBUG

// Timer configuration for PWM (Pulse Width Modulation), which controls the brightness of the grow lights,
// determined by the ratio of on-time to off-time
const Timer_A_UpModeConfig upConfig_led = {
    .clockSource = TIMER_A_CLOCKSOURCE_SMCLK,              // Use system master clock
    .clockSourceDivider = TIMER_A_CLOCKSOURCE_DIVIDER_1,   // Don't divide the clock
    .timerPeriod = PWM_PERIOD,                             // How long each PWM cycle takes
    .timerInterruptEnable_TAIE = TIMER_A_TAIE_INTERRUPT_ENABLE,        // Enable timer interrupts
    .captureCompareInterruptEnable_CCR0_CCIE = TIMER_A_CCIE_CCR0_INTERRUPT_DISABLE,  // Disable CCR0 interrupts
    .timerClear = TIMER_A_DO_CLEAR                         // Clear timer when starting
};

// PWM compare configuration, which determines the brightness level
Timer_A_CompareModeConfig compareConfig_led = {
    .compareRegister = TIMER_A_CAPTURECOMPARE_REGISTER_1,               // Use CCR1 register
    .compareInterruptEnable = TIMER_A_CAPTURECOMPARE_INTERRUPT_ENABLE,  // Enable interrupts
    .compareOutputMode = TIMER_A_OUTPUTMODE_RESET_SET,                  // Reset/Set output mode for PWM
    .compareValue = 0                                                   // Start with 0% duty cycle (off)
};
#endif

/**
 * @brief Sets up the entire grow light system
 * 
 * This function is called once at startup to initialize everything:
 * - Set up communication with the light sensor
 * - Configure the pins that control our LED lights
 * - Set up the timer for PWM brightness control
 * - Create a scheduled task that will run our light control logic
 */
void grow_light_init() {
#ifndef SOFTWARE_DEBUG
    // STEP 1: Initialize I2C communication for the OPT3001 light sensor
    // I2C is a communication protocol that lets us talk to the sensor
    I2C_setslave(OPT3001_SLAVE_ADDRESS);        // Tell I2C which device to talk to
    I2C_write16(CONFIG_REG, DEFAULT_CONFIG);    // Send configuration settings to sensor

    // STEP 2: Configure GPIO (General Purpose Input/Output) pins
    // These pins control our physical LED lights
    
    // Set up the PWM output pin (P7.7) to connect to Timer_A1.1
    GPIO_setAsPeripheralModuleFunctionOutputPin(CCR1_PORT, CCR1_PIN, GPIO_PRIMARY_MODULE_FUNCTION);
    
    // Set up the basic LED control pin (P1.0) as a simple output
    GPIO_setAsOutputPin(LED_PORT, LED_PIN);
    
    // Make sure the LED starts in the OFF state
    GPIO_setOutputLowOnPin(LED_PORT, LED_PIN);

    // STEP 3: Initialize and configure Timer_A1 for PWM control
    // PWM (Pulse Width Modulation) controls brightness by rapidly switching on/off
    Timer_A_configureUpMode(TIMER_A1_BASE, &upConfig_led);     // Set up the timer
    Timer_A_initCompare(TIMER_A1_BASE, &compareConfig_led);    // Set up PWM compare

    // STEP 4: Create and schedule the light update task
    // This task will run every 10.5 seconds to check light levels and adjust our grow lights
    STask light_task = {
        .func = update_light,                // Function to call
        .max_time = TASK_INTERVAL_MS,        // How often to run (10.5 seconds)
        .time_until_process = TASK_INTERVAL_MS,  // Delay before first run
        .active = true                       // Task is enabled
    };
    
    // Add our task to the system scheduler and remember where it is
    gl.stack_pos = push_task(light_task);

#ifdef DEBUG
    puts("Added light task to scheduler");   // Debug message (only shows if DEBUG is enabled)
#endif
#endif

#ifdef DEBUG
    puts("GrowLight initialized\n");         // Debug message showing initialization is complete
#endif
}

static uint32_t apply_sensor_scaling(uint32_t sensor_val, uint32_t exponent) {
    // Use a switch statement to handle different scaling factors
    // The sensor uses powers of 2 for scaling (dividing or multiplying by 2, 4, 8, etc.)
    switch (exponent) {
        case 0:  return sensor_val >> 6;  // Divide by 64 (shift right 6 positions)
        case 1:  return sensor_val >> 5;  // Divide by 32 (shift right 5 positions)
        case 2:  return sensor_val >> 4;  // Divide by 16 (shift right 4 positions)
        case 3:  return sensor_val >> 3;  // Divide by 8  (shift right 3 positions)
        case 4:  return sensor_val >> 2;  // Divide by 4  (shift right 2 positions)
        case 5:  return sensor_val >> 1;  // Divide by 2  (shift right 1 position)
        case 6:  return sensor_val;       // Multiply by 1 (no change)
        case 7:  return sensor_val << 1;  // Multiply by 2  (shift left 1 position)
        case 8:  return sensor_val << 2;  // Multiply by 4  (shift left 2 positions)
        case 9:  return sensor_val << 3;  // Multiply by 8  (shift left 3 positions)
        case 10: return sensor_val << 4;  // Multiply by 16 (shift left 4 positions)
        case 11: return sensor_val << 5;  // Multiply by 32 (shift left 5 positions)
        default: return sensor_val;       // If exponent is invalid, return original value
    }
}

static uint32_t process_sensor_data(uint32_t raw) {
    // Extract the actual light reading from the lower 12 bits
    // The & operator with SENSOR_VALUE_MASK keeps only the bits we want
    uint32_t sensor_val = raw & SENSOR_VALUE_MASK;
    
    // Extract the scaling exponent from the upper 4 bits
    // We shift right to move the upper bits down, then mask to keep only 4 bits
    uint32_t exponent = (raw >> SENSOR_EXPONENT_SHIFT) & SENSOR_EXPONENT_MASK;
    
    // Apply the scaling and return the final value
    return apply_sensor_scaling(sensor_val, exponent);
}

static uint32_t calculate_brightness(uint32_t sensor_val) {
    // If there's enough natural light, don't turn on our grow lights
    if (sensor_val >= gl.threshold) {
        return 0;  // Lights off
    }
    
    // Calculate brightness based on how much darker it is than our threshold
    // This gives us a value from 0 to MAX_BRIGHTNESS
    return ((gl.threshold - sensor_val) * MAX_BRIGHTNESS) / gl.threshold;
}

static void send_light_level_data(uint32_t sensor_val) {
    // Only send data when we're using real hardware (not in software debug mode)
#ifndef SOFTWARE_DEBUG
    // Determine what category of light level we have and send appropriate code
    if (sensor_val < MIN_BRIGHTNESS) {
        // It's dark - send "dark" signal
        send_data(IOT_LIGHT_SENSOR_ID, 0, IOT_LIGHT_DARK);
    } else if (sensor_val < MAX_BRIGHTNESS) {
        // It's medium brightness - send "medium" signal  
        send_data(IOT_LIGHT_SENSOR_ID, 0, IOT_LIGHT_MEDIUM);
    } else {
        // It's bright - send "bright" signal
        send_data(IOT_LIGHT_SENSOR_ID, 0, IOT_LIGHT_BRIGHT);
    }
#endif
}

static void control_hardware_light(bool should_be_on) {
#ifndef SOFTWARE_DEBUG
    // Only change hardware state if we need to (avoid unnecessary operations)
    if (should_be_on && !gl.on) {
        // Turn lights ON
        gl.on = true;  // Update our state tracking
        Timer_A_startCounter(TIMER_A1_BASE, TIMER_A_UP_MODE);  // Start PWM timer
    } else if (!should_be_on && gl.on) {
        // Turn lights OFF
        gl.on = false;  // Update our state tracking
        Timer_A_stopTimer(TIMER_A1_BASE);              // Stop PWM timer
        Timer_A_clearInterruptFlag(TIMER_A1_BASE);     // Clear any pending interrupts
        GPIO_setOutputLowOnPin(LED_PORT, LED_PIN);     // Make sure LED pin is off
    }
#else
    // In software debug mode, just update our state variable
    gl.on = should_be_on;
#endif
}

/********************************************
 *                  GETTER FUNCTIONS       *
 ********************************************/

uint32_t grow_light_get_brightness() {
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
 *                  SETTER FUNCTIONS       *
 ********************************************/
void grow_light_set_brightness(uint32_t brightness) {
    // Doesn't do anything if brightness isn't actually changing
    if (gl.current_brightness == brightness) {
        return;  // Exit early - no work needed
    }
    
    // Update our internal state
    gl.current_brightness = brightness;

#ifndef SOFTWARE_DEBUG
    // Update the hardware PWM timer with the new brightness value
    // This actually changes how bright the LED lights are
    Timer_A_setCompareValue(TIMER_A1_BASE, TIMER_A_CAPTURECOMPARE_REGISTER_1, brightness);
#endif

#ifdef DEBUG
    // Print debug message (only appears if DEBUG is enabled during compilation)
    printf("PWM Brightness set to %u\n", brightness);
#endif
}

void grow_light_set_threshold(uint32_t new_threshold) {
    
    // Making sure the threshold is within acceptable limits
    if (new_threshold < MIN_BRIGHTNESS) {
        #ifdef DEBUG
        printf("New threshold is too low %d, setting to minimum\n", new_threshold);
        #endif
        new_threshold = MIN_BRIGHTNESS;  // Clamp to minimum allowed value
    } else if (new_threshold > MAX_BRIGHTNESS) {
        #ifdef DEBUG
        printf("New threshold is too high %d, setting to maximum\n", new_threshold);
        #endif
        new_threshold = MAX_BRIGHTNESS;  // Clamp to maximum allowed value
    }

    // Updating internal state with the validated value
    gl.threshold = new_threshold;

#ifdef DEBUG
    printf("Threshold set to %d\n", gl.threshold);
#endif
}

void grow_light_set_mode(int32_t manual_mode) {
    // Convert the input parameter to a clear boolean value
    bool enable_manual = (manual_mode != 0);
    
    // Doesn't do anything if mode isn't actually changing
    if (enable_manual == gl.manual_mode) {
        return;  // Exit early - already in the requested mode
    }
    
    // Update our internal state
    gl.manual_mode = enable_manual;

#ifdef DEBUG
    printf("Manual mode %s\n", enable_manual ? "enabled" : "disabled");
#endif

#ifndef SOFTWARE_DEBUG
    // Control the automatic timer based on the mode
    if (enable_manual) {
        // Entering manual mode: Stop the automatic timer
        Timer_A_stopTimer(TIMER_A1_BASE);           // Stop the timer
        Timer_A_clearInterruptFlag(TIMER_A1_BASE);  // Clear any pending interrupts
    } else {
        // Entering automatic mode: Start the automatic timer
        Timer_A_startCounter(TIMER_A1_BASE, TIMER_A_UP_MODE);
    }
#endif
}

void power_on_or_off(int32_t on) {
    // Safety check: Only allow manual control when in manual mode
    if (!gl.manual_mode) {
        return;  // Exit early - manual control not allowed in automatic mode
    }

    // Convert input parameter to a clear boolean value
    bool desired_state = (on != 0);
    
    // Optimization: Don't do anything if lights are already in the desired state
    if (gl.on == desired_state) {
        return;  // Exit early - no change needed
    }

    // Update our internal state
    gl.on = desired_state;

#ifndef SOFTWARE_DEBUG
    // Control the actual hardware based on desired state
    if (desired_state) {
        // Turn lights ON: Set the GPIO pin high (3.3V)
        GPIO_setOutputHighOnPin(LED_PORT, LED_PIN);
    } else {
        // Turn lights OFF: Set the GPIO pin low (0V)
        GPIO_setOutputLowOnPin(LED_PORT, LED_PIN);
    }
#endif

#ifdef DEBUG
    printf("Manual light turned %s\n", desired_state ? "on" : "off");
#endif
}

/************************************
 * MAIN AUTOMATIC CONTROL FUNCTIONS *
 ************************************/

#ifndef SOFTWARE_DEBUG

void update_light() {
    // If we're in manual mode, the user is controlling the lights directly
    if (gl.manual_mode) {
        return;  // Exit early - manual mode is active
    }

    // STEP 1: Read raw sensor data from the light sensor
    I2C_setslave(OPT3001_SLAVE_ADDRESS);    // Select our light sensor for communication
    int32_t raw = I2C_read16(RESULT_REG);   // Read the raw 16-bit sensor value
    
    // STEP 2: Process the raw sensor data into a meaningful light level
    uint32_t sensor_val = process_sensor_data(raw);
    
#ifdef DEBUG
    // Show the processed sensor reading (only if debug is enabled)
    printf("Sensor value: %d\n", sensor_val);
#endif

    // STEP 3: Calculate how bright our grow lights should be
    uint32_t calculated_brightness = calculate_brightness(sensor_val);
    
    // STEP 4: Determine if lights should be on (any brightness > 0 means "on")
    bool should_be_on = (calculated_brightness > 0);
    
    // STEP 5: Control the hardware and update brightness
    control_hardware_light(should_be_on);           // Turn lights on/off as needed
    grow_light_set_brightness(calculated_brightness); // Set the exact brightness level
    
    // STEP 6: Send information to IoT systems
    send_light_level_data(sensor_val);
}
#else

void update_light_hal(uint32_t raw) {
    // Safety check: Only operate when in automatic mode
    if (gl.manual_mode) {
        return;  // Exit early - manual mode is active
    }

    // STEP 1: Process the simulated sensor data into a meaningful light level
    uint32_t sensor_val = process_sensor_data(raw);
    
    // STEP 2: Display the processed sensor reading (always show in simulation mode)
    printf("Sensor value: %d\n", sensor_val);

    // STEP 3: Calculate how bright our grow lights should be
    uint32_t calculated_brightness = calculate_brightness(sensor_val);
    
    // STEP 4: Determine if lights should be on
    bool should_be_on = (calculated_brightness > 0);
    
    // STEP 5: Update our simulated hardware state and brightness
    control_hardware_light(should_be_on);           // Updates gl.on variable
    grow_light_set_brightness(calculated_brightness); // Updates gl.current_brightness
    
    // Note: We don't send IoT data in simulation mode, as there's no real hardware
}

#endif

#ifndef SOFTWARE_DEBUG

void TA1_N_IRQHandler(void) {
    // Handle CCR1 interrupt (timer reached the "compare" value)
    // This happens when it's time to turn the LED OFF during a PWM cycle
    if (Timer_A_getCaptureCompareInterruptStatus(TIMER_A1_BASE,
                                                 TIMER_A_CAPTURECOMPARE_REGISTER_1,
                                                 TIMER_A_CAPTURECOMPARE_INTERRUPT_FLAG)) {
        // Clear the interrupt flag (tell hardware we handled this interrupt)
        Timer_A_clearCaptureCompareInterrupt(TIMER_A1_BASE,
                                             TIMER_A_CAPTURECOMPARE_REGISTER_1);
        // Turn the LED off for the remainder of this PWM cycle
        GPIO_setOutputLowOnPin(LED_PORT, LED_PIN);
    }
    
    // Handle timer overflow interrupt (timer reached its maximum and reset to 0)
    // This happens at the start of each PWM cycle - time to turn LED ON
    if (TIMER_A1->CTL & TIMER_A_CTL_IFG) {
        // Clear the interrupt flag
        TIMER_A1->CTL &= ~TIMER_A_CTL_IFG;
        // Turn the LED on for the start of this PWM cycle
        GPIO_setOutputHighOnPin(LED_PORT, LED_PIN);
    }
}

void update_light_timer(int32_t new_timer) {
    // Input validation: Make sure the new timer value makes sense
    if (new_timer > 0) {
        // Update the timer value for our light control task in the scheduler
        // gl.stack_pos remembers where our task is stored in the task list
        task_list.task_array[gl.stack_pos].max_time = new_timer;
    }
    // If new_timer is 0 or negative, we ignore it (invalid input)
}

#endif
