/*
 * BUZZER ALERT SYSTEM
 * 
 * This module controls an audio buzzer for greenhouse environmental alerts.
 * The buzzer provides audio feedback for critical conditions such as:
 * - Temperature outside acceptable range
 * - Poor air quality requiring ventilation
 * 
 * BUZZER OPERATION:
 * The buzzer uses PWM (Pulse Width Modulation) to generate audio tones.
 * A timer creates a square wave signal at 1 kHz frequency, which produces
 * an audible tone when connected to a piezo buzzer.
 * 
 * CONTROL MODES:
 * - Automatic Mode: Buzzer activates based on environmental conditions
 * - Manual Mode: User has direct control over buzzer on/off state
 * 
 * HARDWARE CONFIGURATION:
 * - Connected to pin P8.2 (Timer A3, CCR2)
 * - Uses Timer A3 for PWM generation
 * - 1 kHz tone frequency (3 MHz clock / 3000 = 1 kHz)
 * - 50% duty cycle for clear audio output
 */

#include "environment_systems/buzzer.h"
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

// HARDWARE-SPECIFIC INCLUDES (only when using hardware)
#ifndef SOFTWARE_DEBUG
#include <ti/devices/msp432p4xx/driverlib/driverlib.h>
#endif

#ifndef SOFTWARE_DEBUG

/*
 * BUZZER TIMER CONFIGURATION
 * 
 * This configuration sets up Timer A3 to generate the base timing for our PWM signal.
 * The timer counts from 0 up to a specified period value, then resets to 0.
 * When used with compare/capture registers, this creates precise timing for audio generation.
 * 
 * FREQUENCY CALCULATION:
 * - System Clock (SMCLK): 3 MHz
 * - Timer Period: 3000 counts
 * - Output Frequency: 3 MHz / 3000 = 1 kHz
 * 
 * A 1 kHz tone is in the audible range and provides clear alert notification
 * without being too harsh or annoying for continuous operation.
 */
Timer_A_UpModeConfig buzzerTimerConfig = {
    .clockSource = TIMER_A_CLOCKSOURCE_SMCLK,              // Use 3 MHz system master clock
    .clockSourceDivider = TIMER_A_CLOCKSOURCE_DIVIDER_1,   // No clock division
    .timerPeriod = 3000,                                   // Count to 3000 for 1 kHz frequency
    .timerInterruptEnable_TAIE = TIMER_A_TAIE_INTERRUPT_DISABLE,       // Disable timer overflow interrupts
    .captureCompareInterruptEnable_CCR0_CCIE = TIMER_A_CCIE_CCR0_INTERRUPT_DISABLE,  // Disable CCR0 interrupts
    .timerClear = TIMER_A_DO_CLEAR                         // Clear timer when configuring
};

/*
 * BUZZER PWM CONFIGURATION
 * 
 * This configuration sets up the PWM (Pulse Width Modulation) output that actually
 * drives the buzzer. PWM works by rapidly switching the output pin high and low.
 * The ratio of high-time to total period determines the "duty cycle."
 * 
 * For audio generation:
 * - We use 50% duty cycle (1500 out of 3000 counts)
 * - This creates a square wave with equal high and low periods
 * - Square waves are efficient for driving piezo buzzers
 * - The toggle mode automatically creates the square wave pattern
 */
Timer_A_CompareModeConfig buzzerPWMConfig = {
    .compareRegister = BUZZER_CCR,                         // Use CCR2 for P8.2 output
    .compareInterruptEnable = TIMER_A_CAPTURECOMPARE_INTERRUPT_DISABLE,  // No interrupts needed
    .compareOutputMode = TIMER_A_OUTPUTMODE_TOGGLE_SET,    // Toggle output on compare match
    .compareValue = 1500                                   // 50% duty cycle (1500/3000)
};

#endif

/*
 * BUZZER INITIALIZATION FUNCTION
 */

void init_buzzer() {
    
#ifndef SOFTWARE_DEBUG
    // STEP 1: GPIO CONFIGURATION
    // Configure pin P8.2 to work with Timer A3's CCR2 output, 
    // which connects the timer's PWM output directly to the physical buzzer pin
    
    GPIO_setAsPeripheralModuleFunctionOutputPin(
        BUZZER_PORT,                    // Port P8
        BUZZER_PIN,                     // Pin 2
        GPIO_PRIMARY_MODULE_FUNCTION    // Primary function = Timer A3.2 output
    );
    
    // STEP 2: TIMER CONFIGURATION
    // Set up Timer A3 in "up mode" where it counts from 0 to the period value
    // This creates the base timing for our 1 kHz audio frequency
    Timer_A_configureUpMode(BUZZER_TIMER, &buzzerTimerConfig);
    
    // STEP 3: PWM COMPARE CONFIGURATION
    // Set up the compare/capture register that creates the actual PWM output
    // This determines when during each timer cycle the output pin toggles
    Timer_A_initCompare(BUZZER_TIMER, &buzzerPWMConfig);
    
    // STEP 4: ENSURE BUZZER STARTS IN OFF STATE
    // Stop the timer to ensure no PWM output (buzzer silent)
    Timer_A_stopTimer(BUZZER_TIMER);
    
#endif

    // STEP 5: INITIALIZE SOFTWARE STATE VARIABLES
    // Set buzzer state to off (no audio output)
    is_on = false;
    
    // Set buzzer to automatic mode (responds to environmental conditions)
    // In automatic mode, the buzzer will activate based on temperature and air quality
    buzzer_manual_mode = false;

#ifdef DEBUG
    puts("Initialized buzzer\n");
#endif
}

/*
 * BUZZER STATE QUERY FUNCTIONS
 */

bool is_buzzer_on() {
    return is_on;
}

bool get_buzzer_manual_mode() {
    return buzzer_manual_mode;
}

/*
 * BUZZER MODE CONTROL FUNCTION
 */

void set_buzzer_mode(int32_t mode) {
    if (mode == 0) {
        // Switch to automatic mode
        buzzer_manual_mode = false;
    } else {
        // Switch to manual mode
        buzzer_manual_mode = true;
    }
    
#ifdef DEBUG
    printf("Buzzer manual mode set to: %s\n", mode ? "true" : "false");
#endif
}

/*
 * BUZZER ACTIVATION FUNCTION
 */

void turn_on_buzzer() {
    // Avoiding redundant activation
    // If buzzer is already on, no need to start it again
    // This prevents timer conflicts and unnecessary hardware operations
    if (!is_on) {
        // Update internal state to reflect buzzer is now active
        is_on = true;

#ifndef SOFTWARE_DEBUG
        // Starting the PWM timer, to begin the automatic PWM generation that drives the buzzer
        // The timer will continue running until explicitly stopped
        Timer_A_startCounter(BUZZER_TIMER, TIMER_A_UP_MODE);
#endif

#ifdef DEBUG
        // Debug output for development and troubleshooting
        puts("Activating buzzer\n");
        puts("BUZZZZZZZZZZZZZZZZZZZZZZZZZZ\n");
#endif
    }
}

/*
 * BUZZER DEACTIVATION FUNCTION
 */

void turn_off_buzzer(int8_t temp, bool air) {
    // Only turns off buzzer if it's currently on AND all environmental conditions are normal
    if (is_on && !air && (temp == 0)) {
        
        // Update internal state to reflect buzzer is now inactive
        is_on = false;

#ifndef SOFTWARE_DEBUG
        // Stopping the PWM signal generation, silencing the buzzer
        // The timer hardware goes into idle state until restarted
        Timer_A_stopTimer(BUZZER_TIMER);
#endif

#ifdef DEBUG
        // Debug message indicating successful deactivation
        puts("Deactivating buzzer\n");
#endif
    }    
}

/*
 * MANUAL BUZZER CONTROL FUNCTION
 */

void manual_power_buzzer(int32_t turn_on) {
    // Only allows manual control when in manual mode, preventing user commands from interfering 
    // with automatic environmental monitoring
    if (buzzer_manual_mode) {
        
        if (turn_on == 0) { // USER WANTS TO TURN BUZZER OFF
            // Use the standard turn_off function with "all clear" parameters
            // temp=0: Indicates no temperature problems
            // air=false: Indicates no air quality problems
            // This ensures proper state management and logging
            turn_off_buzzer(0, false);
            
        } else { // USER WANTS TO TURN BUZZER ON
            // Use the standard activation function for consistency
            // This ensures proper state tracking and hardware control
            turn_on_buzzer();
        }
    }
}
