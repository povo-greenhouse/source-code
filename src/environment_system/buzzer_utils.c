#include "environment_systems/buzzer_utils.h"
#include <stdbool.h>
#include <stdint.h>

#ifndef SOFTWARE_DEBUG
#include <ti/devices/msp432p4xx/driverlib/driverlib.h>
#endif

/// @brief Flag to indicate if the buzzer is currently on. Prevents multiple activations or deactivations of the buzzer.
static bool is_on = false;

// Configuration for the PWM signal to control the buzzer
Timer_A_PWMConfig pwmConfig ={
   TIMER_A_CLOCKSOURCE_SMCLK,       // Use SMCLK as the clock source
   TIMER_A_CLOCKSOURCE_DIVIDER_1,   // No clock divider
   PWM_FREQ,                        // PWM period (ticks)
   BUZZER_CCR,                      // Use CCR2 (TA2.2)
   TIMER_A_OUTPUTMODE_RESET_SET,    // PWM output mode
   PWM_FREQ / 2                     // 50% duty cycle
};

void init_buzzer(){    
    // Map P2.7 to TA2.2 and load CCRx with period & duty
    MAP_GPIO_setAsPeripheralModuleFunctionOutputPin(
        BUZZER_PORT,
        BUZZER_PIN,
        GPIO_PRIMARY_MODULE_FUNCTION
    );
    
    // Initializing CCR for the buzzer
    Timer_A_generatePWM(BUZZER_TIMER, &pwmConfig);
    // Stop the timer in up mode, to turn off the buzzer initially
    MAP_Timer_A_stopTimer(BUZZER_TIMER);

}

void turn_on_buzzer(){
    // check if buzzer is not already on
    if(!is_on){
        is_on = true;

#ifndef SOFTWARE_DEBUG
        pwmConfig.dutyCycle = PWM_FREQ / 2;  // turn on 50%
        Timer_A_generatePWM(BUZZER_TIMER, &pwmConfig);

        // start the timer to activate the buzzer
        Timer_A_startCounter(TIMER_A0_BASE, TIMER_A_UP_MODE);
#endif
#ifdef DEBUG
            // Debug message to indicate the buzzer is activated
            printf("Activating buzzer\n");
            printf("BUZZZZZZZZZZZZZZZZZZZZZZZZZZ\n");
#endif
    }
}

void turn_off_buzzer(int8_t temp, bool air){
    // checking if the buzzer is off
    if(is_on){
        // checking if the air and temperature systems do not need the buzzer to be on
        // ie. air == false and temp == 0
        if(!air && (temp == 0)){
            is_on = false;

#ifndef SOFTWARE_DEBUG
            // Deactivating the buzzer
            // Option A: just stop the timer
            Timer_A_stopTimer(BUZZER_TIMER);

            // Option B: zero out the duty cycle if you want to keep the timer running
            // pwmConfig.dutyCycle = 0;
            // Timer_A_generatePWM(BUZZER_TIMER, &pwmConfig);
#endif

#ifdef DEBUG
            // Debug message to indicate the buzzer is deactivated as the temperature is within the acceptable range
            printf("Deactivating buzzer\n");
#endif
        }
    }
}
