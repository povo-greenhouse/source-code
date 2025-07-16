#include "environment_systems/buzzer.h"
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

#ifndef SOFTWARE_DEBUG
#include <ti/devices/msp432p4xx/driverlib/driverlib.h>
#endif

#ifndef SOFTWARE_DEBUG
// Timer configuration for the buzzer
Timer_A_UpModeConfig buzzerTimerConfig = {
    TIMER_A_CLOCKSOURCE_SMCLK,            // 3MHz clock source
    TIMER_A_CLOCKSOURCE_DIVIDER_1,        // No division, 3 MHz
    3000 ,                                // For 1 kHz tone (3 MHz / 3000 = 1 kHz)
    TIMER_A_TAIE_INTERRUPT_DISABLE,       // Diasbling interupt to timer A0
    TIMER_A_CCIE_CCR0_INTERRUPT_DISABLE,  // Disabling interrupt for capture compare register 0
    TIMER_A_DO_CLEAR                      // Clear
};

// PWM configuration for the buzzer, set up to toggle the output on P8.2. This creates a square wave signal at 1 kHz with a 50% duty cycle.
Timer_A_CompareModeConfig buzzerPWMConfig = {
    BUZZER_CCR,                                   // CCR2 for P8.2
    TIMER_A_CAPTURECOMPARE_INTERRUPT_DISABLE,     // Disable interrupt
    TIMER_A_OUTPUTMODE_TOGGLE_SET,                // Toggle output
    1500                                          // 50% duty cycle
};

#endif

void init_buzzer(){
#ifndef SOFTWARE_DEBUG
    // Setting P8.2 to the primary function ie, handling TimerA3 with capture compare register 2 (CCR2)
    GPIO_setAsPeripheralModuleFunctionOutputPin(
        BUZZER_PORT,
        BUZZER_PIN,
        GPIO_PRIMARY_MODULE_FUNCTION
    );
    // Initialize the timer in up mode with the specified configuration
    Timer_A_configureUpMode(BUZZER_TIMER, &buzzerTimerConfig);
    // Initialize the compare mode for the buzzer PWM signal
    Timer_A_initCompare(BUZZER_TIMER, &buzzerPWMConfig);
    // Stop the timer in up mode, to turn off the buzzer initially
    Timer_A_stopTimer(BUZZER_TIMER);
    // Intializing the buzzer state to off
    is_on = false;
    // Initialize buzzer manual mode to false ie, automatic mode
    buzzer_manual_mode = false;

#endif

#ifdef DEBUG
    puts("Initalized buzzer\n");
#endif

    return;
}
// @brief Function to check if the buzzer is currently on.
// @return Returns true if the buzzer is on, false otherwise.
bool is_buzzer_on(){
    return is_on;
}

/// @brief Function to check if the buzzer is in manual mode.
/// @return Returns true if the buzzer is in manual mode, false otherwise.
bool get_buzzer_manual_mode(){
    return buzzer_manual_mode;
}

void set_buzzer_mode(int32_t mode){
    // automatic mode ie, false
    if (mode == 0){
        buzzer_manual_mode = false;
    } else { // for manual mode
        buzzer_manual_mode = true;
    }
#ifdef DEBUG
printf("Buzzer manual mode set to: %s\n", mode ? "true" : "false");
#endif
}
void turn_on_buzzer(){
    // check if buzzer is not already on
    if(!is_on){
        is_on = true;

#ifndef SOFTWARE_DEBUG
        // start the timer to activate the buzzer
        Timer_A_startCounter(BUZZER_TIMER, TIMER_A_UP_MODE);
#endif
#ifdef DEBUG
            // Debug message to indicate the buzzer is activated
            puts("Activating buzzer\n");
            puts("BUZZZZZZZZZZZZZZZZZZZZZZZZZZ\n");
#endif
    }
}

void turn_off_buzzer(int8_t temp, bool air){
    // checking if the buzzer is off, and if the air quality and temperature conditions are met to turn it off
    // ie. air == false and temp == 0
    if(is_on && !(air) && (temp == 0)){
        is_on = false;

#ifndef SOFTWARE_DEBUG

        Timer_A_stopTimer(BUZZER_TIMER);

#endif

#ifdef DEBUG
        // Debug message to indicate the buzzer is deactivated as the temperature is within the acceptable range
        puts("Deactivating buzzer\n");
#endif
    }
}

void manual_power_buzzer(int32_t turn_on){
    if(buzzer_manual_mode){
        if(turn_on == 0){
            turn_off_buzzer(0, false);
        } else {
            turn_on_buzzer();
        }
    }
}
