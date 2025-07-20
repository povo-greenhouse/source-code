#ifndef ENVIRONMENT_SYSTEM_BUZZER_H_
#define ENVIRONMENT_SYSTEM_BUZZER_H_
#include <stdbool.h>
#include <stdint.h>

#ifndef SOFTWARE_DEBUG
/***********************
 * pins for the buzzer *
 ***********************/
#define BUZZER_PORT GPIO_PORT_P8
#define BUZZER_PIN  GPIO_PIN2

/*************************************
 * defining variables for the timers *
 *************************************/
#define BUZZER_TIMER TIMER_A3_BASE
#define BUZZER_CCR   TIMER_A_CAPTURECOMPARE_REGISTER_2
#endif

/**********************************
 * buzzer state tracking variables *
 **********************************/
static bool is_on;
/// @brief Flag to indicate if the buzzer is in manual mode. 
/// If true, the buzzer will not automatically turn on or off based on system conditions.
static bool buzzer_manual_mode;

/**
 * @brief Initializes the buzzer system for audio alert generation
 * 
 * This function sets up all the hardware and software components needed for
 * buzzer operation. It configures GPIO pins, timer settings, PWM output,
 * and initializes the buzzer to a known off state.
 * 
 * INITIALIZATION SEQUENCE:
 * 1. Configure GPIO pin for timer output
 * 2. Set up timer for PWM generation
 * 3. Configure PWM compare settings
 * 4. Initialize buzzer to off state
 * 5. Set default mode to automatic
 */
void init_buzzer();

/**
 * @brief Turns on the buzzer to generate audio alert
 * 
 * This function starts the buzzer's audio output by enabling the PWM timer.
 * It includes protection against multiple activation attempts and provides
 * debug feedback when the buzzer activates.
 * 
 * HOW PWM AUDIO GENERATION WORKS:
 * 1. Timer A3 counts from 0 to 3000 repeatedly (1 kHz frequency)
 * 2. When count reaches 1500 (compare value), output pin toggles
 * 3. This creates a square wave: high for 1500 counts, low for 1500 counts
 * 4. Square wave drives the buzzer, creating audible 1 kHz tone
 * 5. Tone continues until timer is stopped
 * 
 * FEATURES:
 * - Checks if buzzer is already on to prevent redundant operations
 * - Updates internal state tracking for other functions to reference
 * - Provides debug output for system monitoring and troubleshooting
 */
void turn_on_buzzer();

/**
 * @brief Turns off the buzzer based on environmental conditions
 * 
 * This function stops the buzzer's audio output, but only if environmental
 * conditions indicate it's safe to do so. It implements intelligent buzzer
 * control that considers multiple system inputs before deactivating the alert.
 * 
 * CONDITIONAL DEACTIVATION LOGIC:
 * The buzzer will only turn off if ALL of these conditions are met:
 * 1. Air quality is acceptable (air == false, meaning no air quality alert)
 * 2. Temperature is in normal range (temp == 0, meaning no temperature alert)
 * 
 * This ensures that:
 * - Buzzer continues alerting if ANY environmental problem persists
 * - Multiple simultaneous problems (temp + air) keep buzzer active
 * - Buzzer only silences when ALL conditions return to normal
 * 
 * @param temp Temperature status indicator:
 *             0 = temperature is within acceptable range
 *             positive = temperature too high
 *             negative = temperature too low
 * @param air Air quality alert status:
 *            false = air quality is acceptable
 *            true = air quality requires attention (poor ventilation, etc.)
 */
void turn_off_buzzer(int8_t, bool);

/**
 * @brief Checks if the buzzer is currently producing sound
 * 
 * This function allows other parts of the system to determine the current
 * buzzer state without directly accessing internal variables.
 * 
 * @return true if buzzer is currently on, false if silent
 */
bool is_buzzer_on();


/**
 * @brief Checks if the buzzer is in manual control mode
 * 
 * The buzzer can operate in two modes:
 * - Automatic Mode: Buzzer activates based on environmental conditions
 *   (temperature out of range, poor air quality, etc.)
 * - Manual Mode: User has direct control and can turn buzzer on/off
 *   regardless of environmental conditions
 * 
 * This function lets other parts of the system know which mode is active
 * so they can respond appropriately.
 * 
 * @return true if in manual mode (user control), false if automatic mode
 */
bool get_buzzer_manual_mode();

/**
 * @brief Switches the buzzer between automatic and manual control modes
 * 
 * This function allows the user (through the menu system) to change how
 * the buzzer operates:
 * 
 * AUTOMATIC MODE (mode = 0):
 * - Buzzer activates automatically when environmental conditions require alerts
 * - System controls buzzer based on temperature and air quality readings
 * - User cannot directly turn buzzer on/off in this mode
 * 
 * MANUAL MODE (mode != 0):
 * - User has direct control over buzzer on/off state
 * - Environmental conditions do not automatically trigger the buzzer
 * - Useful when user wants to silence alerts
 * - User can turn buzzer on/off through the menu system
 * 
 * @param mode Control mode selector:
 *             0 = automatic mode (environmental control)
 *             any other value = manual mode (user control)
 */
void set_buzzer_mode(int32_t);

/**
 * @brief Provides direct user control over buzzer when in manual mode
 * 
 * This function allows users to directly turn the buzzer on or off through
 * the menu system, but only when the buzzer is in manual mode.
 * 
 * @param turn_on User's desired buzzer state:
 *                0 = turn buzzer off
 *                any other value = turn buzzer on
 */
void manual_power_buzzer(int32_t);
#endif
