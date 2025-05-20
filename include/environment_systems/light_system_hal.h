#ifndef LIGHT_SYSTEM_HAL_H
#define LIGHT_SYSTEM_HAL_H
#include <stdint.h>
#include <stdbool.h>

#define MAX_BRIGHTNESS 255
#define MIN_BRIGHTNESS 0

typedef struct GrowLight{
    uint8_t current_brightness;
    uint8_t threshold;
    bool on;
    bool manual_mode;
    void (*init)(struct GrowLight *);
    void (*set_brightness)(struct GrowLight *, uint8_t);
    uint8_t (*get_brightness)(struct GrowLight *);
    void (*set_threshold)(struct GrowLight *, uint8_t);
    uint8_t (*get_threshold)(struct GrowLight *);
    void (*set_manual_mode)(struct GrowLight *, bool);
    bool (*get_manual_mode)(struct GrowLight *);
    void (*turn_on)(struct GrowLight *, bool);
    bool (*is_on)(struct GrowLight *);
}GrowLight;

GrowLight* GrowLight_init();
void update_light_intensity(GrowLight*,uint8_t sensor_val);
void update_light_system(GrowLight*, uint8_t);

#endif
