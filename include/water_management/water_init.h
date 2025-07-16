/*
 * water_init.h
 *
 *  Created on: 24 giu 2025
 *      Author: vince
 */



#ifndef INCLUDE_WATER_MANAGEMENT_WATER_INIT_H_
#define INCLUDE_WATER_MANAGEMENT_WATER_INIT_H_
#include "option_menu/options.h"
int state_r;



struct WaterOptionValues{
    bool manual_mode;
    bool pump1_enabled;
    bool pump2_enabled;
    int32_t reservoire_empty_threshold;
    int32_t reservoire_low_threshold;
    int32_t reservoire_moderate_threshold;
    int32_t tank_empty_threshold;
    int32_t enable_pump1_time;
    int32_t enable_pump2_time;
    int32_t disable_pump1_time;
    int32_t disable_pump2_time;
    int32_t read_tank_time;
    int32_t read_reservoire_time;

};
volatile struct WaterOptionValues water_option_values;

void water_init();

void add_water_options();

#endif /* INCLUDE_WATER_MANAGEMENT_WATER_INIT_H_ */
