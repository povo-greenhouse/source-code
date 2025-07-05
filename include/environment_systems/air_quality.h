#ifndef INCLUDE_ENVIRONMENT_SYSTEMS_AIR_QUALITY_H_
#define INCLUDE_ENVIRONMENT_SYSTEMS_AIR_QUALITY_H_

#include <stdint.h>
#include <stdbool.h>
#include "scheduling/scheduler.h"

#define VREF 3.3f // reference voltage (3.3V)
#define VCC 5.0f // operational power of the MQ135 sensor (taken from the datasheet)
#define RL 20000.0f // load resistance of the MQ135  sensor
#define R0 337000.0f // sensor resistance in clean air (calibrated value)
// M and B are calibration constants specific to the gas being measured
#define M -2.769f // slope of the linear regression line for the MQ135 sensor
#define B 2.067f // y-intercept of the linear regression line for the MQ135 sensor

/// @brief Structure to hold air quality data.
///
/// - threshold: The level over which the air quality is considered poor.
///
/// - current_level: The current air quality level measured by the sensor in parts per million (ppm).
///
/// - stack_pos: position in the stack of the scheduler.
typedef struct Air{
    uint32_t threshold;
    uint32_t current_level;
    task_list_index stack_pos;
}Air;

/// @brief Initialize the air quality monitoring system
///
/// This function sets up the air quality system by:
///
/// 1. Configuring GPIO pin P4.3 for analog input (ADC)
///
/// 2. Setting up the ADC14 module for analog-to-digital conversion
///
/// 3. Creating a periodic task for air quality monitoring and adding it to the scheduler
void air_init();

/// @brief Sets the air quality threshold level.
/// It updates the threshold value used to determine when
/// air quality is poor and an alert should be triggered.
/// Used by the option menu to increase or decrease the 
/// threshold level, as specified by the user.
/// @param level The new threshold value in ppm or sensor units
void air_set_threshold(uint32_t);

/// @brief Sets to the current air quality level 
/// @param level the new current level provided by the sensor
void air_set_level(uint32_t);

/// @brief Gets the current threshold for air quality. 
/// Used by the option menu to visualize the current threshold level.
/// @return the current threshold level used to compare the air quality
uint32_t air_get_threshold();

/// @brief Function to get the current air quality level. Used by the application for monitoring the air quality
/// @return the current air quality level
uint32_t air_get_level();

/// @brief Check if current air quality exceeds the threshold
///
/// It compares the current air quality level with the set threshold
/// and returns true if the level is equal to or above the threshold (poor air quality) 
/// and indicates that the buzzer should be activated. 
/// @return true if the current level exceeds the threshold, false otherwise
bool exceeding_threshold();
#ifndef SOFTWARE_DEBUG
/// @brief Main function to read sensor data, update air quality and handle buzzer state.
/// This function is called periodically by the scheduler to:
///
/// 1. Read the ADC value from the MQ135 sensor
///
/// 2. Convert the ADC reading to a voltage
///
/// 3. Calculate the gas concentration in ppm using sensor calibration
///
/// 4. Update the air quality level
///
/// 5. Check if threshold is exceeded and control the buzzer accordingly
///
/// This function is called periodically by the scheduler to ensure the air quality is monitored continuously. 
void update_air();
#else
/// @brief Hardware-independent function to update air quality level and buzzer state.
/// This function is used in software debugging mode to simulate the air quality update process.
/// It takes a raw sensor value as input, updates the air quality level,
/// and checks if the threshold is exceeded to control the buzzer state.
/// @param level The raw sensor value representing the air quality level
void update_air_hal(uint32_t);
#endif
/// @brief Update the air quality monitoring timer interval
/// It sets the new timer value in the scheduler to allow dynamic adjustment 
/// of how frequently the air quality sensor is read.
/// @param new_timer The new timer interval in milliseconds
void update_air_timer(int32_t);

/// @brief Calibrate the baseline resistance of the MQ135 sensor. 
/// It determines the sensor's resistance in clean air (R0).
/// This value is crucial for accurate gas concentration calculations.
/// The function takes multiple readings and averages them for accuracy.
/// @param no_samples Number of samples to take for calibration averaging 
/// @return: The averaged R0 resistance value
float calibrateR0(int32_t);

#endif