#include "include/environment_systems/co2_hal.h"
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include<stdbool.h>

void co2_init(struct CO2 *self){
    self->current_level = 50; // default level
    self->threshold = 500; // default threshold, need to change based on the CO2 sensor
    #ifdef DEBUG
    printf("CO2 initialized\n");
    #endif

}
void co2_set_threshold(struct CO2 *self, uint16_t level){
    if(self){
        self->threshold = level;
        #ifdef DEBUG
        printf("CO2 threshold set to %d\n", level);
        #endif
    }
}
uint16_t co2_get_threshold(struct CO2 *self){
    if(self){
        return self->threshold;
    }
    return 0;
}
void co2_set_level(struct CO2 *self, uint16_t level){
    if(self){
        self->current_level = level;
        #ifdef DEBUG
        printf("CO2 threshold set to %d\n", level);
        #endif
    }
}
uint16_t co2_get_level(struct CO2 *self){
    if(self){
        return self->current_level;
    }
    return 0;
}
bool exceeding_threshold(struct CO2 *self){
    if(self){
        if(self->current_level >= self->threshold){
            #ifdef DEBUG
            printf("CO2 level %d exceeds threshold %d\n", level, self->threshold);
            #endif
            return true;
        }else{
            #ifdef DEBUG
            printf("CO2 level %d is within threshold %d\n", level, self->threshold);
            #endif
            return false;
        }
    }
    return false;
}
CO2* CO2_init(){
    CO2 *co2 = (CO2*)malloc(sizeof(CO2));
    if(co2){
        co2->init = co2_init;
        co2->set_threshold = co2_set_threshold;
        co2->get_threshold = co2_get_threshold;
        co2->get_current_level = co2_set_level;
        co2->set_current_level = co2_get_level;
        co2->is_exceeding_threshold = exceeding_threshold;
        co2->init(co2);
    }
    return co2;
}