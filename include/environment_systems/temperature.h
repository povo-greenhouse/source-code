#ifndef INCLUDE_ENVIRONMENT_SYSTEMS_TEMPERATURE_H_
#define INCLUDE_ENVIRONMENT_SYSTEMS_TEMPERATURE_H_

#include <stdint.h>

#ifndef SOFTWARE_DEBUG

#include "scheduling/scheduler.h"

/*******************************************
 *  defining the TMP006 register addresses *
 *******************************************/ 
// I2C address of the TMP006 sensor
#define TMP006_SLAVE_ADDRESS    0x40
// register address for the ambient temperature
#define TMP006_P_TABT           0x01
// register address to write to the TMP006 sensor (to set configuration settings)
#define TMP006_WRITE_REG        0x02


/*********************************************************
 *  defining the TMP006 configuration settings registers *
 *********************************************************/ 
// bit to reset the TMP006 sensor
#define TMP006_RST              0x8000
// bit to power down the TMP006 sensor
#define TMP006_POWER_DOWN       0x0000
// bit to power up the TMP006 sensor
#define TMP006_POWER_UP         0x7000
// bit to set the conversion rate to 2Hz
#define TMP006_CR_2             0x0200

#endif

/**********************************************
 *  defining the default temperature settings *
 **********************************************/

#define DEFAULT_LOWER_THRESHOLD 20 // Default lower temperature threshold
#define DEFAULT_HIGHER_THRESHOLD 30 // Default higher temperature threshold
#define DEFAULT_TEMP 21 // Default temperature value

/**
 * @brief Structure to hold all the data and settings for our temperature monitoring system.
 * 
 * - current_temperature: The current temperature reading from the sensor.
 *
 * - higher_threshold: The upper limit for the temperature range.
 *
 * - lower_threshold: The lower limit for the temperature range.
 *
 * - stack_pos: position in the stack of the scheduler.
*/
typedef struct TemperatureSensor{
    int8_t current_temperature;
    int8_t higher_threshold;
    int8_t lower_threshold;

#ifndef SOFTWARE_DEBUG
    task_list_index stack_pos;
#endif

}TemperatureSensor;

/**
 * @brief Initializes the temperature monitoring system
 * 
 * This function sets up all components needed for automatic temperature monitoring:
 * 1. Configures I2C communication with the TMP006 sensor
 * 2. Initializes the sensor with proper settings
 * 3. Creates a scheduled task for periodic temperature readings
 * 4. Integrates with the system scheduler for automatic operation
 */
void temp_sensor_init();

/**
 * @brief Sets the lower temperature threshold for greenhouse monitoring
 * 
 * This function safely updates the minimum acceptable temperature.
 * SAFETY VALIDATION:
 * The function ensures the lower threshold is always less than the upper threshold
 * to maintain a valid temperature range. Invalid settings are rejected.
 * 
 * @param new_threshold The new minimum temperature
 */
void temp_set_lower_threshold(uint8_t);

/**
 * @brief Retrieves the current lower temperature threshold
 * 
 * This getter function allows other parts of the system to check
 * the current minimum temperature setting for:
 * - Display in user interfaces
 * - Validation in configuration routines
 * 
 * @return Current lower threshold value (0-255°C)
 */
uint8_t temp_get_lower_threshold();

/**
 * @brief Sets the upper temperature threshold for greenhouse monitoring
 * 
 * This function safely updates the maximum acceptable temperature.
 *
 * SAFETY VALIDATION:
 * The function ensures the upper threshold is always greater than the lower threshold
 * to maintain a valid temperature range. Invalid settings are rejected.
 * 
 * @param new_threshold The new maximum temperature
 */
void temp_set_higher_threshold(uint8_t);

/**
 * @brief Retrieves the current upper temperature threshold
 * 
 * This getter function allows other parts of the system to check
 * the current maximum temperature setting for:
 * - Display in user interfaces  
 * - Validation in configuration routines
 * - Data logging and monitoring
 * 
 * @return Current upper threshold value (0-255°C)
 */
uint8_t temp_get_higher_threshold();

/**
 * @brief Retrieves the most recent temperature reading
 * 
 * This function returns the last temperature measurement taken by
 * the TMP006 infrared sensor.
 * 
 * @return Current temperature value
 * 
 * TEMPERATURE SCALING:
 * The sensor provides readings in Celsius with 0.03125°C resolution
 */
uint8_t temp_get_current_temperature();

/**
 * @brief Updates the current temperature reading in system memory
 * 
 * This function stores a new temperature measurement and triggers
 * debug logging if enabled. It's called by:
 * 1. Automatic sensor reading routines (every 5.5 seconds)
 * 2. Manual temperature input during software debugging
 * 3. External temperature data sources (if implemented)
 * 
 * @param temperature New temperature value to store (0-255°C)
 * 
 * SYSTEM INTEGRATION:
 * After updating the temperature, the system automatically:
 * - Compares against thresholds (Goldilocks check)
 * - Triggers alerts if outside acceptable range
 * - Sends data to IoT monitoring systems
 */
void temp_set_current_temperature(uint8_t);

/// It compares the current temperature with the lower and higher thresholds.
/// @return - -1 if below the lower threashold
///  
///  -  1 if above the higher threshold
///  
///  -  0 if within the acceptable range

/**
 * @brief Determines if current temperature is within the acceptable range.
 * 
 * This function implements the core logic of greenhouse temperature monitoring
 * by evaluating the current temperature against defined thresholds. 
 * 
 * PRINCIPLE:
 * - TOO COLD ie. -1 (< lower_threshold): Heating needed, plants may suffer frost damage
 * - TOO HOT ie. +1 (> higher_threshold): Cooling needed, plants may wilt or burn
 * - JUST RIGHT ie. 0 (within range): Perfect growing conditions maintained
 * 
 * AUTOMATED RESPONSES:
 * Each condition triggers different system actions:
 * 1. Buzzer alerts for out-of-range conditions
 * 2. IoT data transmission with status codes
 * 
 * @return Temperature evaluation result.
 */
int8_t would_goldilocks_like_this();

/**
 * @brief Main temperature monitoring function
 * 
 * This is the heart of the automatic temperature monitoring system.
 * It performs a complete temperature monitoring cycle:
 * 
 * 1. Sensor Reading: Get current temperature from TMP006 infrared sensor
 * 2. Data Processing: Convert raw sensor data to usable temperature values  
 * 3. Threshold Evaluation: Apply Goldilocks principle to determine status
 * 4. Alert Management: Control buzzer based on temperature conditions
 * 5. IoT Integration: Send status updates to external monitoring systems
 * 
 * AUTOMATION INTEGRATION:
 * This function integrates with multiple greenhouse systems:
 * - Buzzer alerts for immediate operator notification
 * - IoT data transmission for remote monitoring
 * - Future expansion: heating/cooling system control
 * - Data logging for historical analysis and optimization
 */
void update_temperature();

#ifndef SOFTWARE_DEBUG
/**
 * @brief Updates the temperature monitoring interval dynamically
 * 
 * This function allows real-time adjustment of how frequently the system
 * checks temperature readings.
 * 
 * SYSTEM INTEGRATION:
 * The function directly modifies the task scheduler's timing for the temperature
 * monitoring task, enabling immediate adjustment without system restart.
 * 
 * @param new_timer New monitoring interval in milliseconds
 */
void update_temperature_timer(int32_t);
#endif //end of if guard

#endif // end of file guard
