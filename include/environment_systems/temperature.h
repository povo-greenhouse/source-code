#ifndef INCLUDE_ENVIRONMENT_SYSTEMS_TEMPERATURE_H_
#define INCLUDE_ENVIRONMENT_SYSTEMS_TEMPERATURE_H_

#include <stdint.h>

#ifndef SOFTWARE_DEBUG
#define TMP006_SLAVE_ADDRESS    0x40
// TMP006 register addresses
#define TMP006_P_TABT           0x01
#define TMP006_WRITE_REG        0x02
// TMP006 configuration settings registers
#define TMP006_RST              0x8000
#define TMP006_POWER_DOWN       0x0000
#define TMP006_POWER_UP         0x7000
#define TMP006_CR_2             0x0200

// pins for the buzzer
#define BUZZER_PORT     P2
#define BUZZER_PIN      BIT7
#endif

typedef struct TemperatureSensor{
    int8_t current_temperature;
    int8_t higher_threshold;
    int8_t lower_threshold;
}TemperatureSensor;

void temp_sensor_init();
void temp_set_lower_threshold(uint8_t);
uint8_t temp_get_lower_threshold();
void temp_set_higher_threshold(uint8_t);
uint8_t temp_get_higher_threshold();
uint8_t temp_get_current_temperature();
void temp_set_current_temperature(uint8_t);
int8_t would_goldilocks_like_this();
void update_temperature();
#endif
