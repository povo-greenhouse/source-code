///*
// * water_reading.c
// *
// *  Created on: 24 giu 2025
// *      Author: vince
// */
//
#include "water_management/water_reading.h"
#include "water_management/pump_management.h"
#include "water_management/water_init.h"
#include "uart_communication/uart_comm.h"
#include "option_menu/option_menu.h"
#include "IOT/IOT_communication.h"
#include "scheduling/scheduler.h"
#include <stdbool.h>
#include <stdio.h>
#include "adc/adc.h"

void read_reservoire() {
    uint32_t res_value = water_arr[0];
    if (res_value < water_option_values.reservoire_empty_threshold) {
        send_data(4, 1, 4);
    #ifdef DEBUG
        printf("Reservoire empty \n");
    #endif
    } else if (res_value < water_option_values.reservoire_low_threshold) {
        send_data(4, 1, 3);
    #ifdef DEBUG
        printf("Low water in the Reservoir \n");
    #endif
    } else if (res_value < water_option_values.reservoire_moderate_threshold) {
        send_data(4, 0, 2);
    #ifdef DEBUG
        printf("Moderate water in the Reservoir \n");
    #endif
    } else {
        send_data(4, 0, 1);
    #ifdef DEBUG
        printf("Reservoire full \n");
    #endif
    }
    #ifdef DEBUG
        printf("Reading executed! \n");
    #endif
}

void read_tank() {
    uint32_t tank_value = water_arr[1];
    if (tank_value > water_option_values.tank_empty_threshold) {
        P4->OUT &= ~BIT7;
        send_data(3, 1, 0);
        block = true;
    #ifdef DEBUG
            printf("Tank full! pumps have been blocked\n");
    #endif
        } else {
            block = false;
            send_data(3, 0, 0);
    #ifdef DEBUG
            printf("Tank empty! good to go\n");
    #endif
        }
}

void add_water_reading_options(){
    int error;
    OptionUnion res_empty_thresh = option_u_new_threshold(RESERVOIRE_EMPTY_THRESH_DEFAULT,0,20,1,&error);

    option_menu_push_option(option_new("Empty Reservoire Threshold",
                                                           THRESHOLD,
                                                           res_empty_thresh,
                                             upd_res_empty_threshold,
                                         to_string_threshold_default));
    OptionUnion res_low_thresh = option_u_new_threshold(RESERVOIRE_LOW_THRESH_DEFAULT,0,500,2,&error);


    option_menu_push_option(option_new("Low Reservoire Threshold",
                                                           THRESHOLD,
                                                           res_low_thresh,
                                             upd_res_low_threshold,
                                         to_string_threshold_default));

    OptionUnion res_moderate_thresh = option_u_new_threshold(RESERVOIRE_MODERATE_THRESH_DEFAULT,0,500,2,&error);


    option_menu_push_option(option_new("Moderate Reservoire Threshold",
                                                           THRESHOLD,
                                                           res_moderate_thresh,
                                             upd_res_moderate_threshold,
                                         to_string_threshold_default));

    OptionUnion tank_empty_thresh = option_u_new_threshold(TANK_EMPTY_THRESH_DEFAULT,0,500,2,&error);


    option_menu_push_option(option_new("Empty Tank Threshold",
                                                           THRESHOLD,
                                                           tank_empty_thresh,
                                             upd_tank_empty_threshold,
                                         to_string_threshold_default));


    OptionUnion read_tank_time = option_u_new_timer(timer_option_get_name_by_value(READ_TANK_TIME_DEFAULT),&error);

    option_menu_push_option(option_new("Tank Reading Time",
                                                     TIMER,
                                                     read_tank_time,
                                                     upd_tank_read_time,
                                                     to_string_timer_default));
    OptionUnion read_reservoire_time = option_u_new_timer(timer_option_get_name_by_value(READ_RESERVOIRE_TIME_DEFAULT),&error);

    option_menu_push_option(option_new("Reservoire Reading Time",
                                                           TIMER,
                                                           read_reservoire_time,
                                                           upd_res_read_time,
                                                           to_string_timer_default
                                       ));


}

void upd_res_empty_threshold(int32_t val){
    water_option_values.reservoire_empty_threshold = val;
}
void upd_res_low_threshold(int32_t val){
    water_option_values.reservoire_low_threshold = val;
}
void upd_res_moderate_threshold(int32_t val){
    water_option_values.reservoire_moderate_threshold = val;
}
void upd_tank_empty_threshold(int32_t val){
    water_option_values.tank_empty_threshold = val;
}

void upd_tank_read_time(int32_t val){
    water_option_values.read_tank_time=val;
    task_list.task_array[index_tank].max_time = val;
}
void upd_res_read_time(int32_t val){
    water_option_values.read_reservoire_time = val;
    task_list.task_array[index_reservoire].max_time=val;

}

void handle_water_level_interrupt(uint64_t status){
    water_arr[0] = (int32_t)ADC14_getResult(TANK_SENSOR_MEM);
    water_arr[1] = (int32_t)ADC14_getResult(RESERVOIRE_SENSOR_MEM);
}
