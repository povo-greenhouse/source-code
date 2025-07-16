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

/// @brief Flag to indicate if the buzzer is currently on. Prevents multiple activations or deactivations of the buzzer.
static bool is_on;
/// @brief Flag to indicate if the buzzer is in manual mode. 
/// If true, the buzzer will not automatically turn on or off based on system conditions.
static bool buzzer_manual_mode;

/// @brief Initializes the buzzer by configuring the GPIO pin and setting up the PWM signal.
/// This function maps the buzzer pin to the appropriate timer output and initializes the PWM configuration.
/// It also stops the timer initially to ensure the buzzer is off when the system starts.
void init_buzzer();

/// @brief Function to turn on the buzzer.
/// This function checks if the buzzer is not already on before activating it.
void turn_on_buzzer();

/// @brief Fuction to turn off the buzzer.
/// This function checks if the buzzer is currently on and if the air and temperature systems do
/// not require the buzzer to be on. If both conditions are met, it deactivates the buzzer.
/// @param temp The current temperature range reading to check if the buzzer should be active. If it is 0 (within normal range), the buzzer can be turned off.
/// @param air A boolean indicating whether the air system is needs the buzzer to be active. If false, the buzzer can be turned off. 
void turn_off_buzzer(int8_t, bool);

/// @brief Function to check if the buzzer is currently on.
/// @return Returns true if the buzzer is on, false otherwise.
bool is_buzzer_on();


/// @brief Function to check if the buzzer is in manual mode.
/// @return Returns true if the buzzer is in manual mode, false otherwise.
bool get_buzzer_manual_mode();

/// @brief Function to set the buzzer to manual mode.
/// @param mode A boolean indicating whether to set the buzzer to manual mode (true)
/// or automatic mode (false).
void set_buzzer_mode(int32_t);

/// @brief Function to power the buzzer on or off.
/// It is used to control the buzzer's state based on user input.
/// @param on A boolean indicating whether to turn the buzzer on (true) or off (false).
void manual_power_buzzer(int32_t);
#endif
