#ifndef CO2_HAL_H
#define CO2_HAL_H
#include <stdint.h>
#include <stdbool.h>

typedef struct CO2{
    uint16_t threshold;
    uint32_t current_level;
    void (*init)(struct CO2 *self);
    void (*set_threshold)(struct CO2 *self, uint16_t new_threshold);
    uint16_t (*get_threshold)(struct CO2 *self);
    uint32_t (*get_current_level)(struct CO2 *self);
    void (*set_current_level)(struct CO2 *self, uint32_t level);
    bool (*is_exceeding_threshold)(struct CO2 *self);
}CO2;
CO2* CO2_init();
#endif