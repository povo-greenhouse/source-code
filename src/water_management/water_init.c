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



void water_init(){
    state_r =4;

    pump_init();
    adc_init();

      STask task1 = {
          activatePump1,
          10000,
          10000,
          true
      };

      STask task2 = {
          turnOffPump1,
          20000,
          20000,
          false
      };


      STask task3 = {
            activatePump2,
            10000,
            10000,
            true
        };

        STask task4 = {
            turnOffPump2,
            10000,
            10000,
            false
        };


      STask task5 = {
            start_adc_conversion,
            100,
            100,
            true
        };

      STask task6 = {
             read_tank_level,
             300,
             300,
             false
         };
      STask task7 = {
           read_reservoir_level,
           300,
           300,
           false
       };

      activate_pump1_index = push_task(task1);
      deactivate_pump1_index=push_task(task2);
      activate_pump2_index = push_task(task3);
      deactivate_pump2_index=push_task(task4);
      index_adc=push_task(task5);
      index_checks=push_task(task6);
      index_reservoire=push_task(task7);
}
