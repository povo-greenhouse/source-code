#ifndef INCLUDE_ENVIRONMENT_SYSTEMS_AIR_QUALITY_H_
#define INCLUDE_ENVIRONMENT_SYSTEMS_AIR_QUALITY_H_

#include <stdint.h>
#include <stdbool.h>
#include "scheduling/scheduler.h"

/// @brief Structure to hold air quality data.
///
/// - threshold: The level over which the air quality is considered poor.
///
/// - current_level: The current air quality level measured by the sensor.
///
/// - stack_pos: position in the stack of the scheduler.
typedef struct Air{
    uint32_t threshold;
    uint32_t current_level;
    task_list_index stack_pos;
}Air;

/// @brief Sets up the GPIO pin for the air quality sensor, 
/// creates a task to update air quality and pushing it onto the scheduler.
void air_init();

/// @brief Sets the threshold for air quality. Used by the option menu to increase or decrease the threshold level, 
/// as specified by the user.
/// @param level the new threshold level to set
void air_set_threshold(uint32_t);

/// @brief Sets to the current air quality level 
/// @param level the current level provided by the sensor
void air_set_level(uint32_t);

/// @brief Function to get the threshold for air quality. Used by the option menu to visualize the current threshold level.
/// @return the current threshold level used to compare the air quality
uint32_t air_get_threshold();

/// @brief Function to get the current air quality level. Used by the application for monitoring the air quality
/// @return the current air quality level
uint32_t air_get_level();

/// @brief Function to check if the current air quality level exceeds the threshold
/// @note If the current level is greater than or equal to the threshold, it indicates poor air quality 
/// and the buzzer should be activated.
/// @return true if the current level exceeds the threshold, false otherwise
bool exceeding_threshold();

/// @brief Function to update the air quality level and handle buzzer state.
/// It reads the air quality level from the sensor at pin 4.7, updates the air quality level,
/// checks if the level exceeds the threshold, and manages the buzzer state accordingly.
///
/// This function is called periodically by the scheduler to ensure the air quality is monitored continuously. 
void update_air();

/// @brief Function to update the timer associated with the update air quality sensor values task.
/// It sets the new timer value in the scheduler.
void update_air_timer(int32_t);

#endif
