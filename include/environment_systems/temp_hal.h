#ifndef TEMP_HAL_H
#define TEMP_HAL_H
#include <stdint.h>
typedef struct TemperatureSensor{
    float current_temperature;
    float higher_threshold; // may have to convert to int8_t
    float lower_threshold; // this one too
    void (*init)(struct TemperatureSensor *self);
    void (*set_lower_threshold)(struct TemperatureSensor *self, float new_threshold);
    float (*get_lower_threshold)(struct TemperatureSensor *self);
    float (*get_higher_threshold)(struct TemperatureSensor *self);
    void (*set_higher_threshold)(struct TemperatureSensor *self, float new_threshold);
    float (*get_current_temperature)(struct TemperatureSensor *self);
    void (*set_current_temperature)(struct TemperatureSensor *self, float temperature);
    int8_t (*would_goldilocks_like_this)(struct TemperatureSensor *self);
}TemperatureSensor;
TemperatureSensor* Temperature_init();
#endif