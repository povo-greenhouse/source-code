#include "light_system_hal.h"
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
void growing_light_init(GrowLight *self) {
    if (self) {
        self->current_brightness = 0;
        self->threshold = 100; //default threshold, need to change based on the light sensor
        self->on=false;
        self->manual_mode = false;
        #ifdef DEBUG
        printf("GrowLight initialized\n");
        #endif
    }
}

void growing_light_set_brightness(GrowLight *self, uint8_t brightness) {
    if(self){
        if(brightness > MAX_BRIGHTNESS){
            self->current_brightness = MAX_BRIGHTNESS;
        } else if (brightness < MIN_BRIGHTNESS) {
            self->current_brightness = MIN_BRIGHTNESS;
        } else {
            self->current_brightness = brightness;
        }
        #ifdef DEBUG
        printf("Brightness set to %d\n", self->current_brightness);
        #endif
    }
}

uint8_t growing_light_get_brightness(GrowLight *self) {
    if (self) {
        return self->current_brightness;
    }
    return 0;
}

void growing_light_set_threshold(GrowLight *self, uint8_t new_threshold) {
    if (self) {
        self->threshold = new_threshold;
        #ifdef DEBUG
        printf("Threshold set to %d\n", new_threshold);
        #endif
    }
}

uint8_t growing_light_get_threshold(GrowLight *self) {
    if(self){
        return self->threshold;
    }
    return 0;
}

void growing_light_set_mode(GrowLight *self, bool manual_mode) {
    if(self){
        self->manual_mode = manual_mode;
        #ifdef DEBUG
        if(manual_mode) {
            printf("Manual mode set");
        } else {
            printf("Automatic mode set");
        }
        #endif
    }    
}

bool growing_light_get_mode(GrowLight *self) {
    if(self){
        return self->manual_mode;
    }
    return false;
}

void growing_light_set_power(GrowLight *self, uint8_t power_mode) {
    if(self && self->on != power_mode){
        self->on = power_mode;
        #ifdef DEBUG
        if(power_mode) {
            printf("Light turned on\n");
        } else {
            printf("Light turned off\n");
        }
        #endif
    }
}

bool growing_light_get_power(GrowLight *self) {
    if (self) {
        return self->on;
    }
    return 0;
}

GrowLight* GrowLight_init() {
    GrowLight *light = (GrowLight *)malloc(sizeof(GrowLight));
    if (light) {
        light->init = growing_light_init;
        light->set_brightness = growing_light_set_brightness;
        light->get_brightness = growing_light_get_brightness;
        light->set_threshold = growing_light_set_threshold;
        light->get_threshold = growing_light_get_threshold;
        light->set_manual_mode = growing_light_set_mode;
        light->get_manual_mode = growing_light_get_mode;
        light->turn_on = growing_light_set_power;
        light->is_on = growing_light_get_power;

        light->init(light);
    }
    return light;
}
void update_light_intensity(GrowLight* system, uint8_t sensor_val){
    if(system && system->is_on(system)){
        uint32_t calculated_brightness = ((uint32_t)(system->threshold - sensor_val) * MAX_BRIGHTNESS) / system->threshold;
        system->set_brightness(system, calculated_brightness);
        #ifdef DEBUG
        printf("Light intensity updated to %d\n", calculated_brightness);
        #endif
    }
}
void update_light_system(GrowLight* system, uint8_t sensor_val){
    if(system && !system->get_manual_mode(system)){
        if(system->is_on(system)){
            if(sensor_val > system->get_threshold(system)){
                system->turn_on(system, false); // turn off the light
            }else{
                update_light_intensity(system, sensor_val);
            }
        }else{
            if(sensor_val < system->get_threshold(system)){
                system->turn_on(system, true); // turn on the light
                update_light_intensity(system, sensor_val);
            }
        }
    }
}