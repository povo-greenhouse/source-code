#ifndef INCLUDE_LIGHT_SYSTEM_GROWING_LIGHT_H_
#define INCLUDE_LIGHT_SYSTEM_GROWING_LIGHT_H_

#include <stdint.h>
#include <stdbool.h>

#ifndef SOFTWARE_DEBUG
#include "scheduling/scheduler.h"

// GPIO pin for the LED light
#define LED_PORT GPIO_PORT_P1
#define LED_PIN GPIO_PIN0

// GPIO pin for the PWM output (CCR1)
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

#endif

// Default light threshold value (when to turn lights on/off)
#define DEFAULT_THRESHOLD 1500

// How often our light update task runs (in milliseconds)
#define TASK_INTERVAL_MS 10500         // 10.5 seconds

/***************************************************************************
 *  PWM (Pulse Width Modulation) settings for controlling light brightness *
 ***************************************************************************/
#define PWM_FREQUENCY_HZ 1200          // How fast PWM pulses (1200 times per second)
#define SMCLK_FREQUENCY_MHZ 3000000    // Microcontroller clock speed (3 MHz)
#define PWM_PERIOD (SMCLK_FREQUENCY_MHZ / PWM_FREQUENCY_HZ)  // Calculate PWM period

/*****************************************
 *  Bit masks for processing sensor data *
 *****************************************/
// These help us extract specific parts of the sensor reading
#define SENSOR_EXPONENT_MASK 0x000F    // Last 4 bits (0000111100001111 in binary)
#define SENSOR_VALUE_MASK 0x0FFF       // First 12 bits (000011111111111 in binary)
#define SENSOR_EXPONENT_SHIFT 12       // How many positions to shift to get exponent

/************************************
 *  Constants for IoT communication *
 ************************************/
// These numbers represent different light levels we send to other systems
#define IOT_LIGHT_SENSOR_ID 5          // ID number for our light sensor
#define IOT_LIGHT_DARK 1               // Code for "dark" condition
#define IOT_LIGHT_MEDIUM 2             // Code for "medium light" condition
#define IOT_LIGHT_BRIGHT 3             // Code for "bright light" condition

/******************************************************
 *  defining the threshold values for the grow light *
 ******************************************************/
// maximum brightness value for the grow light
#define MAX_BRIGHTNESS 3000
// minimum brightness value for the grow light
#define MIN_BRIGHTNESS 500


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
#ifndef SOFTWARE_DEBUG
    task_list_index stack_pos;
#endif
}GrowLight;

/**
 * @brief Applies scaling factor to sensor value based on exponent
 * 
 * This function does the math to convert the light sensor data, which has a special format, by scaling
 * the reading based on an exponent value.
 * 
 * @param sensor_val Raw sensor value from the lower 12 bits of sensor data
 * @param exponent Scaling exponent from upper 4 bits of sensor data
 * @return Scaled sensor value that represents actual light level
 */
static uint32_t apply_sensor_scaling(uint32_t, uint32_t);

/**
 * @brief Processes raw sensor data from OPT3001 light sensor
 * 
 * The sensor gives us a 16-bit number that contains both the light reading
 * and scaling information packed together. This function separates them.
 * 
 * @param raw Raw 16-bit value from sensor (contains both data and scaling info)
 * @return Processed sensor value that represents actual light level
 */
static uint32_t process_sensor_data(uint32_t);

/**
 * @brief Calculates how bright our grow lights should be
 * 
 * This function implements our main logic: when it's dark outside,
 * turn on the grow lights. The darker it is, the brighter our lights should be.
 * 
 * @param sensor_val Current light level reading from the sensor
 * @return Calculated brightness value [0 to MAX_BRIGHTNESS]
 *         0 = lights off, MAX_BRIGHTNESS = full brightness
 */
static uint32_t calculate_brightness(uint32_t sensor_val);

/**
 * @brief Sends light level information to IoT (Internet of Things) systems
 * 
 * This function tells other connected systems about the current light conditions.
 * For example, it might send data to a phone app or web dashboard.
 * 
 * @param sensor_val Current light level reading from our sensor
 */
static void send_light_level_data(uint32_t sensor_val);

/**
 * @brief Controls the physical LED lights and PWM timer
 * 
 * This function actually turns the hardware on or off. It manages both
 * the timer (for PWM brightness control) and the basic on/off state.
 * 
 * @param should_be_on true if lights should be on, false if they should be off
 */
static void control_hardware_light(bool should_be_on);

/**
 * @brief Sets up the entire grow light system
 * 
 * This function is called once at startup to initialize everything:
 * - Set up communication with the light sensor
 * - Configure the pins that control our LED lights
 * - Set up the timer for PWM brightness control
 * - Create a scheduled task that will run our light control logic
 */
void grow_light_init();

/**
 * @brief Gets the current brightness level of our grow lights
 * @return Current brightness value (0 = off, higher = brighter)
 */
uint32_t grow_light_get_brightness();

/**
 * @brief Gets the light threshold value
 * @return Threshold value (when natural light is below this, our grow lights turn on)
 */
uint32_t grow_light_get_threshold();

/**
 * @brief Checks if the grow lights are currently on
 * @return true if lights are on, false if they're off
 */
bool is_grow_light_on();

/**
 * @brief Checks if the system is in manual mode
 * @return true if in manual mode (user controls lights), false if automatic mode
 */
bool grow_light_get_mode();

/**
 * @brief Sets the brightness level of our grow lights
 * 
 * This function updates both our internal tracking and the actual hardware.
 * It includes an optimization to avoid unnecessary updates.
 * 
 * @param brightness Desired brightness level (0 = off, higher = brighter)
 */
void grow_light_set_brightness(uint32_t brightness);

/**
 * @brief Sets the light threshold value (when to turn grow lights on/off)
 * 
 * The threshold determines at what natural light level our grow lights should activate.
 * This function includes safety checks and only allows changes in manual mode.
 * 
 * @param new_threshold New threshold value to set
 */
void grow_light_set_threshold(uint32_t);

/**
 * @brief Switches between manual and automatic control modes
 * 
 * Manual mode: User has direct control over the lights
 * Automatic mode: System automatically controls lights based on sensor readings
 * 
 * @param manual_mode 0 = automatic mode, any other value = manual mode
 */
void grow_light_set_mode(int32_t);

/**
 * @brief Sets the brightness level of our grow lights
 * 
 * This function updates both our internal tracking and the actual hardware.
 * It includes an optimization to avoid unnecessary updates.
 * 
 * @param brightness Desired brightness level (0 = off, higher = brighter)
 */
void grow_light_set_brightness(uint32_t);

/**
 * @brief Gets the current brightness level of our grow lights
 * @return Current brightness value (0 = off, higher = brighter)
 */
uint32_t grow_light_get_brightness();

/**
 * @brief Sets the light threshold value (when to turn grow lights on/off)
 * 
 * The threshold determines at what natural light level our grow lights should activate.
 * This function includes safety checks and only allows changes in manual mode.
 * 
 * @param new_threshold New threshold value to set
 */
void grow_light_set_threshold(uint32_t);

/**
 * @brief Gets the light threshold value
 * @return Threshold value (when natural light is below this, our grow lights turn on)
 */
uint32_t grow_light_get_threshold();

/**
 * @brief Switches between manual and automatic control modes
 * 
 * Manual mode: User has direct control over the lights
 * Automatic mode: System automatically controls lights based on sensor readings
 * 
 * @param manual_mode 0 = automatic mode, any other value = manual mode
 */
void grow_light_set_mode(int32_t manual_mode);

/**
 * @brief Checks if the system is in manual mode
 * @return true if in manual mode (user controls lights), false if automatic mode
 */
bool grow_light_get_mode();

/**
 * @brief Checks if the grow lights are currently on
 * @return true if lights are on, false if they're off
 */
bool is_grow_light_on();

/**
 * @brief Manually turns the grow lights on or off
 * 
 * This function allows direct user control of the lights, but only works
 * when the system is in manual mode. This prevents conflicts with automatic control.
 * 
 * @param on 0 = turn lights off, any other value = turn lights on
 */
void power_on_or_off(int32_t);

#ifndef SOFTWARE_DEBUG
/**
 * @brief Main automatic light control function (runs on real hardware)
 * 
 * This function is called automatically every 10.5 seconds by the task scheduler.
 * It reads the light sensor, calculates appropriate brightness, and controls the grow lights.
 * This is the "brain" of our automatic lighting system.
 */
void update_light();
#else
/**
 * @brief Hardware-independent version of automatic light control (for testing/simulation)
 * 
 * This function does the same logic as update_light() but works without real hardware.
 * It's used for testing our algorithms and debugging our code on a regular computer.
 * 
 * @param raw Simulated raw sensor value (normally this would come from the actual sensor)
 */
void update_light_hal(uint32_t);
#endif

#ifndef SOFTWARE_DEBUG

/**
 * @brief Interrupt handler for Timer_A1 (handles PWM timing)
 * 
 * This function is called automatically by the microcontroller hardware
 * when timer events occur. It's responsible for creating the PWM signal
 * that controls LED brightness by switching the lights on and off rapidly.
 * 
 * Interrupts are like "emergency phone calls" - the processor stops what
 * it's doing to handle them immediately, then returns to normal operation.
 */
void TA1_N_IRQHandler(void);

/**
 * @brief Updates how often the automatic light control runs
 * 
 * This function allows changing the timing of our automatic light checking.
 * By default, we check every 10.5 seconds, but this can be adjusted.
 * 
 * @param new_timer New interval in milliseconds (must be > 0)
 */
void update_light_timer(int32_t);
#endif // end of if guard

#endif // end of file guard
