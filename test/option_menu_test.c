/*
 * option_menu_test.c
 *
 *  Created on: May 24, 2025
 *      Author: riginel
 */

#include "include/option_menu/option_menu.h"
#include "include/option_menu/options.h"
#include "include/scheduling/scheduler.h"
#include "test/scheduling_test.h"
#include "test/option_menu_test.h"
#include <stdint.h>
task_list_index a,b;

void blink_routine_a_on_change(int32_t value){
    task_list.task_array[a].max_time = value;
}
void blink_routine_b_on_change(int32_t value){
    task_list.task_array[b].max_time = value;
}
void option_menu_test_main(){
    //REQUIRED: scheduling_test

    sched_test_configurePorts();
    //creating the two tasks
    STask task =  {
            blink_routine,
            1000,
            1000,
            true
    };

    STask task2 = {
           blink_routine_b,
           30000,
           30000,
           true
    };
    a = push_task(task);
    b = push_task(task2);
    int err_a,err_b;

    //OptionList * option_menu = &option_list;
    //creating two new timer options, with 1s and 2s respectively
    OptionUnion option_u_a = option_u_new_timer("1s",&err_a),option_u_b = option_u_new_timer("30s",&err_b);


    option_menu_push_option(
            option_new("blink a timer",
                       TIMER,
                       option_u_a,
                       blink_routine_a_on_change,
                       to_string_timer_default)
                           );

    option_menu_push_option(
               option_new("blink b timer",
                          TIMER,
                          option_u_b,
                          blink_routine_b_on_change,
                          to_string_timer_default)
                              );

}


