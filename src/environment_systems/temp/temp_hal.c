#include <include/environment_systems/temp_hal.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

void temp_init(TemperatureSensor *self) {
    if (self) {
        self->current_temperature = 21.0; //assuming default temperature
        self->higher_threshold = 40.0;
        self->lower_threshold = 10.0;
        #ifdef DEBUG
        printf("TemperatureSensor initialized\n");
        #endif
    }
}

void temp_set_lower_threshold(TemperatureSensor *self, uint8_t new_threshold){
    if(self){
        if(new_threshold < self->higher_threshold){
            self->lower_threshold = new_threshold;
            #ifdef DEBUG
            printf("Lower threshold set to %f\n", self->lower_threshold);
            #endif
        }
    }
}

float temp_get_lower_threshold(TemperatureSensor *self) {
    if (self) {
        return self->lower_threshold;
    }
    return 0;
}

void temp_set_higher_threshold(TemperatureSensor *self, uint8_t new_threshold){
    if(self){
        if(new_threshold > self->lower_threshold){
            self->higher_threshold = new_threshold;
            #ifdef DEBUG
            printf("Higher threshold set to %f\n", self->higher_threshold);
            #endif
        }
    }
}

float temp_get_higher_threshold(TemperatureSensor *self) {
    if (self) {
        return self->higher_threshold;
    }
    return 0;
}

float temp_get_current_temperature(TemperatureSensor *self) {
    if (self) {
        return self->current_temperature;
    }
    return -10.0; // Error case
}

void temp_set_current_temperature(TemperatureSensor *self, float temperature) {
    if (self) {
        self->current_temperature = temperature;
        #ifdef DEBUG
        printf("Current temperature set to %f\n", self->current_temperature);
        #endif
    }
}

int8_t temp_compare(TemperatureSensor *self) {
    if(self){
        if(self->current_temperature < self->lower_threshold){
            #ifdef DEBUG
            printf("Temperature is too low\n");
            #endif
            return -1;
        } else if(self->current_temperature > self->higher_threshold){
            #ifdef DEBUG
            printf("Temperature is too high\n");
            #endif
            return 1;
        } else {
            #ifdef DEBUG
            printf("Temperature is just right\n");
            #endif
            return 0;
        }
    }
    return -2; // Error case
}

TemperatureSensor* Temperature_init() {
    TemperatureSensor *temp = (TemperatureSensor *)malloc(sizeof(TemperatureSensor));
    if (temp) {
        temp->init = temp_init;
        temp->set_lower_threshold = temp_set_lower_threshold;
        temp->get_lower_threshold = temp_get_lower_threshold;
        temp->get_higher_threshold = temp_get_higher_threshold;
        temp->set_higher_threshold = temp_set_higher_threshold;
        temp->get_current_temperature = temp_get_current_temperature;
        temp->set_current_temperature = temp_set_current_temperature;
        temp->would_goldilocks_like_this = temp_compare;

        temp->init(temp);
    }
    return temp;
}