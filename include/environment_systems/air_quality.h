#ifndef INCLUDE_ENVIRONMENT_SYSTEMS_AIR_QUALITY_H_
#define INCLUDE_ENVIRONMENT_SYSTEMS_AIR_QUALITY_H_

#include <stdint.h>
#include <stdbool.h>

typedef struct Air{
    uint32_t threshold;
    uint32_t current_level;
}Air;

void air_init();
void air_set_threshold(uint32_t);
void air_set_level(uint32_t);
uint32_t air_get_threshold();
uint32_t air_get_level();
bool exceeding_threshold();

#endif
