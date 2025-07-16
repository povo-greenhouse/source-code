/*
 * water_init.c
 *
 *  Created on: 24 giu 2025
 *      Author: vince
 */

#include "water_management/water_init.h"
#include "water_management/pump_management.h"
#include "water_management/water_reading.h"
#include "scheduling/scheduler.h"

void water_option_values_init(){
    water_option_values.manual_mode = false;
    water_option_values.pump1_enabled = false;
    water_option_values.pump2_enabled = false;
    water_option_values.reservoire_empty_threshold = RESERVOIRE_EMPTY_THRESH_DEFAULT;
    water_option_values.reservoire_low_threshold = RESERVOIRE_LOW_THRESH_DEFAULT;
    water_option_values.reservoire_moderate_threshold = RESERVOIRE_MODERATE_THRESH_DEFAULT;
    water_option_values.tank_empty_threshold = TANK_EMPTY_THRESH_DEFAULT;
    water_option_values.enable_pump1_time = PUMP1_ENABLE_TIME_DEFAULT;
    water_option_values.enable_pump2_time = PUMP2_ENABLE_TIME_DEFAULT;
    water_option_values.disable_pump1_time = PUMP1_DISABLE_TIME_DEFAULT;
    water_option_values.disable_pump2_time = PUMP2_DISABLE_TIME_DEFAULT;
    water_option_values.read_tank_time = READ_TANK_TIME_DEFAULT;
    water_option_values.read_reservoire_time = READ_RESERVOIRE_TIME_DEFAULT;
}

void water_init(){
    state_r =4;
    water_option_values_init();


    pump_init();
    //option_menu_adc_init();

      STask task1 = {
          activate_pump1,
          water_option_values.enable_pump1_time,
          water_option_values.enable_pump1_time,
          true
      };

      STask task2 = {
          turn_off_pump1,
          water_option_values.disable_pump1_time,
          water_option_values.disable_pump1_time,
          false
      };


      STask task3 = {
            activate_pump2,
            water_option_values.enable_pump2_time,
            water_option_values.enable_pump2_time,
            true
        };

        STask task4 = {
            turn_off_pump2,
            water_option_values.disable_pump2_time,
            water_option_values.disable_pump2_time,
            false
        };



      STask task5 = {
             read_tank,
             water_option_values.read_tank_time,
             water_option_values.read_tank_time,
             true
         };
      STask task6 = {
           read_reservoire,
           water_option_values.read_reservoire_time,
           water_option_values.read_reservoire_time,
           true
       };

      activate_pump1_index = push_task(task1);
      deactivate_pump1_index=push_task(task2);
      activate_pump2_index = push_task(task3);
      deactivate_pump2_index=push_task(task4);
      index_tank=push_task(task5);
      index_reservoire=push_task(task6);
}

void add_water_options(){


    add_water_reading_options();
    add_pump_options();
}
