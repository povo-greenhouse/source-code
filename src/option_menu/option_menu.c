/*
 * option_menu.c
 *
 *  Created on: May 13, 2025
 *      Author: riginel
 */

#include "include/option_menu/option_menu.h"
int32_t push_option(Option option) {
    if (option_list.len == MAX_OPTIONS) {
        return -1;
    }
    option_list.arr[option_list.len] = option;
    if (current_setting == -1) {
        current_setting = 0;
    }
    return option_list.len++;
}
int32_t pop_option() {
    if (current_setting == option_list.len) {
        current_setting--;
    }
    if (option_list.len == 0) {
        return -1;
    }
    option_list.len--;
    return 1;
}

void init_option_list() {
    option_list.len = 0;
    current_setting = 0;
}

int nav_next_option(){
    if ( current_setting == option_list.len-1){
        return -1;
    }
    return ++current_setting;

}
int nav_prev_option(){
    if(current_setting == -1 || current_setting == 0){
        return -1;
    }
    return --current_setting;
}
