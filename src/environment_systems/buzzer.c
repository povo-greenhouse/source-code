#include "environment_systems/buzzer.h"
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

#ifndef SOFTWARE_DEBUG
#include <ti/devices/msp432p4xx/driverlib/driverlib.h>
#endif

/// @brief Flag to indicate if the buzzer is currently on. Prevents multiple activations or deactivations of the buzzer.
static bool is_on = false;

#ifndef SOFTWARE_DEBUG
// Timer configuration for the buzzer
Timer_A_UpModeConfig buzzerTimerConfig = {
    TIMER_A_CLOCKSOURCE_SMCLK, // 3MHz clock source
    TIMER_A_CLOCKSOURCE_DIVIDER_1, // No division, 3 MHz
    3000 , // For 1 kHz tone (3 MHz / 3000 = 1 kHz)
    TIMER_A_TAIE_INTERRUPT_DISABLE,
    TIMER_A_CCIE_CCR0_INTERRUPT_DISABLE,
    TIMER_A_DO_CLEAR
};

// PWM configuration for the buzzer, set up to toggle the output on P8.2. This creates a square wave signal at 1 kHz with a 50% duty cycle.
Timer_A_CompareModeConfig buzzerPWMConfig = {
    TIMER_A_CAPTURECOMPARE_REGISTER_2,         // CCR2 for P8.2
    TIMER_A_CAPTURECOMPARE_INTERRUPT_DISABLE,
    TIMER_A_OUTPUTMODE_TOGGLE_SET,             // Toggle output
    1500                                       // 50% duty cycle
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
    Timer_A_configureUpMode(TIMER_A3_BASE, &buzzerTimerConfig);
    // Initialize the compare mode for the buzzer PWM signal
    Timer_A_initCompare(TIMER_A3_BASE, &buzzerPWMConfig);
    // Stop the timer in up mode, to turn off the buzzer initially
    Timer_A_stopTimer(BUZZER_TIMER);
#endif

#ifdef DEBUG
    puts("Initalized buzzer\n");
#endif

    return;
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

bool is_buzzer_on(){
    return is_on;
}
