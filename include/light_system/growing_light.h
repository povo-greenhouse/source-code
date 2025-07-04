#ifndef INCLUDE_LIGHT_SYSTEM_GROWING_LIGHT_H_
#define INCLUDE_LIGHT_SYSTEM_GROWING_LIGHT_H_

#include <stdint.h>
#include <stdbool.h>
#include "scheduling/scheduler.h"

#define LED_PORT GPIO_PORT_P1
#define LED_PIN GPIO_PIN0

#define CCR1_PORT GPIO_PORT_P7
#define CCR1_PIN GPIO_PIN7
// address of the OPT3001 light sensor
#define OPT3001_SLAVE_ADDRESS 0x44

/********************************************
 *  defining the OPT3001 register addresses *
 ********************************************/
// register address for the result of the light sensor
#define RESULT_REG 0x00
// register address to write to the OPT3001 sensor (to set configuration settings)
#define CONFIG_REG 0x01

/**********************************************
 *  defining the OPT3001 configuration values *
 **********************************************/
// to configure the sensor to operate for an 800ms conversion time
#define DEFAULT_CONFIG 0xCC10 // 800ms
// to configure the sensor to operate for a 100ms conversion time
#define DEFAULT_CONFIG_100 0xC410 // 100ms

/******************************************************
 *  defining the threshold values for the grow light *
 ******************************************************/
// maximum brightness value for the grow light
#define MAX_BRIGHTNESS 2500
// minimum brightness value for the grow light
#define MIN_BRIGHTNESS 1000

/// @brief Structure to hold the grow light system data.
///
/// - current_brightness: The current brightness level of the grow light.
///
/// - threshold: The threshold value for the grow light to determine whether to the lights
///   should be turned on or off based on the ambient light detected by the sensor.
///
/// - manual_mode: A flag indicating whether the grow light is in manual mode.
///
/// - on: A flag indicating whether the grow light is currently turned on or off.
///
/// - stack_pos: position in the stack of the scheduler
typedef struct GrowLight{
    uint32_t current_brightness;
    uint32_t threshold;
    bool manual_mode;
    bool on;
    task_list_index stack_pos;
}GrowLight;

/// @brief Initializes the grow light system.
/// The function sets up the GPIO pin for the grow light, configures the timer for PWM output,
/// and initializes the OPT3001 light sensor.
/// It also creates a task to update the light system at regular intervals 
/// and pushes it to the scheduler.
void grow_light_init();

/// @brief Sets the brightness of the grow light.
///
/// The brightness value is clamped between the minimum and maximum brightness values. 
/// If the brightness exceeds the maximum value, it is set to the maximum.
/// If it is below the minimum value, it is set to the minimum.
/// @param brightness The desired brightness level for the grow light.
void grow_light_set_brightness(uint32_t);

/// @brief Retrieves the current brightness of the grow light.
///
/// Used by the display and application to visualize the current brightness level of the growing lights.
/// @return Current brightness level of the grow light.
uint32_t grow_light_get_brightness();

/// @brief Sets the threshold for the grow light.
/// If the manual mode is enabled, the threshold value is updated, else it remains unchanged.
///
/// Used by the option menu to set the threshold level for the grow light.
/// @param new_threshold The new threshold value for the grow light.
void grow_light_set_threshold(uint32_t);

/// @brief Retrieves the current threshold for the grow light.
/// Used by the option menu to visualize the current threshold level.
/// @return The current threshold value for the grow light.
uint32_t grow_light_get_threshold();

/// @brief Sets the mode of the grow light system.
/// If manual mode is enabled, the grow light system will be handed to the user to perform manual adjustments.
/// @param  manual_mode mode to set for the grow light system (true for manual mode, false for automatic mode).
void grow_light_set_mode(int32_t);

/// @brief Retrieves the current mode of the grow light system.
/// @return current mode of opertion of the grow light system (true for manual mode, false for automatic mode).
bool grow_light_get_mode();

/// @brief Toggles the power state of the grow light.
/// If the grow light is currently on, it will be turned off throught the GPIO pin, and vice versa.
/// This function also updates the structure value to reflect the current power state of the grow light.
void power_on_or_off(int32_t);

/// @brief Indicates whether the grow light is currently turned on or off.
/// @return true if the grow light is on, false if it is off.
bool is_grow_light_on();

/// @brief Updates the light intensity based on the sensor value.
/// This function calculates the brightness level based on the sensor value and the threshold.
/// If the sensor value is below the threshold, it calculates the brightness level and sets it.
/// If the sensor value is above the threshold, it turns off the grow light and sets the intensity to 0.
///
/// @param sensor_val The current sensor value read from the OPT3001 light sensor.
void update_light_intensity(uint32_t);
#ifndef SOFTWARE_DEBUG

/// @brief Updates the grow light system based on the ambient light sensor reading.
/// This function reads the sensor value from the OPT3001 light sensor, calculates the brightness level,
/// and updates the grow light system accordingly.
/// If the sensor value is below the threshold, it turns on the grow light and sets the brightness.
/// If the sensor value is above the threshold, it turns off the grow light and sets the brightness to 0.
void update_light();
#else

/// @brief The hardware independent version of the update_light function
///  which updates the grow light system based on the ambient light sensor reading.
/// It takes values as a parameter and updates the grow light system accordingly.
/// @param raw The raw sensor value read from the user in hexadecimal format.
void update_light_hal(uint32_t);
#endif

/// @brief Updates the light timer with the specified time.
/// This function sets the timer value for the grow light system, which determines how often the light system is updated.
void update_light_timer(int32_t);

/// @brief Timer_A1 interrupt handler for the grow light system.
/// This function is called when the Timer_A1 interrupt occurs, toggling the LED state.
void TA1_N_IRQHandler(void);
#endif
