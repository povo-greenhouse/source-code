#ifndef INCLUDE_LIGHT_SYSTEM_GROWING_LIGHT_H_
#define INCLUDE_LIGHT_SYSTEM_GROWING_LIGHT_H_

#include <stdint.h>
#include <stdbool.h>

#define OPT3001_SLAVE_ADDRESS 0x44

#define RESULT_REG 0x00
#define CONFIG_REG 0x01

#define DEFAULT_CONFIG 0xCC10 // 800ms
#define DEFAULT_CONFIG_100 0xC410 // 100ms

#define MAX_BRIGHTNESS 2500
#define MIN_BRIGHTNESS 500

typedef struct GrowLight{
    uint32_t current_brightness;
    uint32_t threshold;
    bool manual_mode;
    bool on;
}GrowLight;

void grow_light_init();
void grow_light_set_brightness(uint32_t);
uint32_t grow_light_get_brightness();
void grow_light_set_threshold(uint32_t);
uint32_t grow_light_get_threshold();
void grow_light_set_mode(bool);
bool grow_light_get_mode();
void power_on_or_off();
bool is_grow_light_on();
void update_light_intensity(uint32_t);
#ifndef SOFTWARE_DEBUG
void update_light();
#else
void update_light_hal(uint32_t);
#endif

#endif
