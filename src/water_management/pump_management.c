/*
 * PumpManagement.c
 *
 *  Created on: 24 mag 2025
 *      Author: vince
 */

#include "water_management/pump_management.h"
#include "water_management/water_reading.h"
#include "water_management/water_init.h"
#include "option_menu/options.h"
#include "option_menu/option_menu.h"

#include <stdio.h>
#include <stdbool.h>

//4.7 pump 1
//4.6 pump 2
//5.4 sensor 1
//5.5 sensor 2
#define PUMP1_PORT P4
#define PUMP2_PORT P4
#define PUMP1_PIN BIT7
#define PUMP2_PIN BIT6


void pump_init() {
    printf("Executing: pump_init()\n");
    PUMP1_PORT->DIR |= PUMP1_PIN;
    PUMP1_PORT->OUT &= ~PUMP1_PIN;

    PUMP2_PORT->DIR |= PUMP2_PIN;
    PUMP2_PORT->OUT &= ~PUMP2_PIN;


}

void activate_pump1() {
    if (!block){
        printf("Executing: activatePump1()\n");
        send_data(6, 1, 0);

        PUMP1_PORT->OUT |= PUMP1_PIN; // Modificato da P1->OUT |= BIT5

        disable_task_at(activate_pump1_index);
        enable_task_at(deactivate_pump1_index);
        act_1=true;
    }else{
        printf("Blocked pump 1 act");
    }
}


void turn_off_pump1() {
    if(!block){
        printf("Executing: turnOffPump1()\n");
        send_data(6, 0, 0);

        PUMP1_PORT->OUT &= ~PUMP1_PIN;

        disable_task_at(deactivate_pump1_index);
        enable_task_at(activate_pump1_index);
        act_1=false;
    }else{
        printf("Blocked pump 1 off");
    }
}

void activate_pump2() {

        printf("Executing: activatePump2()\n");
        send_data(7, 1, 0);
        PUMP2_PORT->OUT |= PUMP2_PIN;

        disable_task_at(activate_pump2_index);
        enable_task_at(deactivate_pump2_index);
        act_2=true;
}

void turn_off_pump2() {
    if (!block){
        printf("Executing: turnOffPump2()\n");
        send_data(7, 0, 0);
        PUMP2_PORT->OUT &= ~PUMP2_PIN;
        disable_task_at(deactivate_pump2_index);
        enable_task_at(activate_pump2_index);
        act_2=false;
    }else{
        printf("Blocked pump 2 turnoff");
    }


}
void upd_pump1_enable_time(int32_t val);
void upd_pump2_enable_time(int32_t val);
void upd_pump1_disable_time(int32_t val);
void upd_pump2_disable_time(int32_t val);
void add_pump_options(){
    int error;


    OptionUnion enable_pump1_time = option_u_new_timer(timer_option_get_name_by_value(PUMP1_ENABLE_TIME_DEFAULT),&error);
    option_menu_push_option(option_new("Pump 1 Enable Time",
                                                         TIMER,
                                                         enable_pump1_time,
                                                         upd_pump1_enable_time,
                                                         to_string_timer_default));
    OptionUnion enable_pump2_time = option_u_new_timer(timer_option_get_name_by_value(PUMP2_ENABLE_TIME_DEFAULT),&error);
    option_menu_push_option(option_new("Pump 2 Enable Time",
                                                         TIMER,
                                                         enable_pump2_time,
                                                         upd_pump2_enable_time,
                                                         to_string_timer_default));
    OptionUnion disable_pump1_time = option_u_new_timer(timer_option_get_name_by_value(PUMP1_DISABLE_TIME_DEFAULT),&error);
    option_menu_push_option(option_new("Pump 1 Disable Time",
                                                         TIMER,
                                                         disable_pump1_time,
                                                         upd_pump1_disable_time,
                                                         to_string_timer_default));
    OptionUnion disable_pump2_time = option_u_new_timer(timer_option_get_name_by_value(PUMP2_DISABLE_TIME_DEFAULT),&error);
    option_menu_push_option(option_new("Pump 2 Disable Time",
                                                         TIMER,
                                                         disable_pump2_time,
                                                         upd_pump2_disable_time,
                                                         to_string_timer_default));



}

void upd_pump1_enable_time(int32_t val){
    water_option_values.enable_pump1_time=val;
    task_list.task_array[activate_pump1_index].max_time = val;

}

void upd_pump2_enable_time(int32_t val){
    water_option_values.enable_pump1_time=val;
    task_list.task_array[activate_pump2_index].max_time = val;

}

void upd_pump1_disable_time(int32_t val){
    water_option_values.enable_pump2_time=val;
    task_list.task_array[deactivate_pump1_index].max_time = val;

}

void upd_pump2_disable_time(int32_t val){
    water_option_values.disable_pump2_time=val;
    task_list.task_array[deactivate_pump2_index].max_time = val;

}


