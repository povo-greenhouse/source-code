#ifndef INCLUDE_ENVIRONMENT_SYSTEMS_TEMPERATURE_H_
#define INCLUDE_ENVIRONMENT_SYSTEMS_TEMPERATURE_H_

#include <stdint.h>

#ifndef SOFTWARE_DEBUG
/// defining the I2C address of the TMP006 sensor
#define TMP006_SLAVE_ADDRESS    0x40

/*******************************************
 *  defining the TMP006 register addresses *
 *******************************************/ 
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


/***********************
 * pins for the buzzer *
 ***********************/ 

#define BUZZER_PORT     P2
#define BUZZER_PIN      BIT7
#endif

/// @brief Structure to hold temperature sensor data.
/// 
/// - current_temperature: The current temperature reading from the sensor.
///
/// - higher_threshold: The upper limit for the temperature range.
///
/// - lower_threshold: The lower limit for the temperature range.
typedef struct TemperatureSensor{
    int8_t current_temperature;
    int8_t higher_threshold;
    int8_t lower_threshold;
}TemperatureSensor;

/// @brief Initializes the temperature sensor and buzzer, sets up the I2C communication, 
/// and adds a task to the scheduler for updating the temperature.
void temp_sensor_init();

/// @brief Function to set the lower threshold for temperature monitoring.
/// It updates the lower threshold only if the new threshold is less than the higher threshold.
/// @param new_threshold the new lower threshold value to set
void temp_set_lower_threshold(uint8_t);

/// @brief Function retrieves the current lower threshold for temperature monitoring.
/// Used bu the option menu to visualize the current lower threshold level.
/// @return current lower threshold value
uint8_t temp_get_lower_threshold();

/// @brief Function to set the higher threshold for temperature monitoring.
/// It updates the higher threshold only if the new threshold is greater than the lower threshold.
/// @param new_threshold the new higher threshold value to set
void temp_set_higher_threshold(uint8_t);

/// @brief Function retrieves the current higher threshold for temperature monitoring.
/// Used by the option menu to visualize the current higher threshold level.
/// @return the current higher threshold value
uint8_t temp_get_higher_threshold();

/// @brief Function retrieves the current temperature reading from the temperature sensor.
/// Used by the display and application to visualize the current temperature level.
/// @return the current temperature value
uint8_t temp_get_current_temperature();

/// @brief Function updates the current temperature reading from the temperature sensor.
/// @param temperature the new temperature value to set
void temp_set_current_temperature(uint8_t);

/// @brief Function to determine if the current temperature is within the acceptable range.
/// It compares the current temperature with the lower and higher thresholds.
/// @return - -1 if below the lower threashold
///  
///  -  1 if above the higher threshold
///  
///  -  0 if within the acceptable range
int8_t would_goldilocks_like_this();

/// @brief Function to update the values of the temperature sensor data structure.
/// It reads the ambient temperature from the TMP006 sensor, updates the current temperature,
/// and checks if the temperature is within the acceptable range.
/// The buzzer is activated if the temperature is out of range, to indicate that the temperature is not suitable for the plants.
void update_temperature();
#endif
