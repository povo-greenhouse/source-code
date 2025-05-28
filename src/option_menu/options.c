#include "include/option_menu/options.h"
#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include <stdio.h>

TimerAmount timer_option_arr[TIMER_OPTION_ARR_LEN] = {
    {"2ms",2},
    {"10ms",10},
    {"100ms",100},
    {"500ms",500},
    {"1s",1000},
    {"2s",2000},
    {"5s",5000},
    {"10s",10000},
    {"20s",20000},
    {"30s",30000},
    {"1m",60000},
    {"2m",120000},
    {"5m",300000},
    {"10m",600000},
    {"20m",1200000},
    {"30m",1800000},
    {"1h",3600000},
    {"2h",7200000}
};
char OUT_OF_RANGE[13] =  "out of range";

OptionUnion option_u_new_threshold(int32_t value, int32_t min_value,
                                   int32_t max_value, int32_t increments,
                                   int *error) {
    if (min_value > max_value || (min_value > value || max_value < min_value)) {
        *error = 1;
    }
    OptionUnion u;

    u.threshold = (OptionTU){value, min_value, max_value, increments};
    *error = 0;
    return u;
}
OptionUnion option_u_new_timer(char *name, int *error) {
    OptionUnion u;
    t_arr_index index = timer_option_get_index(name);
    if (index == -1) {
        *error = 1;
    }
    u.timer = index;
    return u;
}
OptionUnion option_u_new_switch(bool sswitch) {
    OptionUnion u;
    u.sswitch = sswitch;
    return u;
}

Option option_new(char *name, OptionType type, OptionUnion value,
                  OnChangeFP action, FmtStringFP to_string) {
    Option ret;
    strncpy(ret.name, name, OPTION_NAME_MAX_LENGTH);
    ret.type = type;
    ret.value = value;
    ret.on_change_action = action;
    ret.to_string = to_string;
    ret.changed = false;
    return ret;
}
int32_t option_get_value(Option *option) {
    switch (option->type) {
    case SWITCH:
        return option->value.sswitch;
    case THRESHOLD:
        return option->value.threshold.value;
    case TIMER:
        return timer_option_get_value(option->value.timer);
    }
}
int32_t option_increment(Option *option) {
    switch (option->type) {
    case SWITCH:
        option->value.sswitch = !option->value.sswitch;
        break;
    case THRESHOLD: {
        OptionTU value = option->value.threshold;
        if (value.max_value <= value.value + value.increments) {
            value.value = value.max_value;
        } else {
            value.value += value.increments;
        }
        option->value.threshold = value;
        break;
    }
    case TIMER: {
        t_arr_index index = option->value.timer;
        if (index >= TIMER_OPTION_ARR_LEN - 1) {
            option->value.timer = TIMER_OPTION_ARR_LEN-1;

        }else{
            option->value.timer++;
        }
        break;
    }
    }
    return 1;
}
int32_t option_decrement(Option *option) {
    switch (option->type) {
    case SWITCH:
        option->value.sswitch = !option->value.sswitch;
        break;
    case THRESHOLD: {
        OptionTU value = option->value.threshold;
        if (value.value - value.min_value < value.increments) {
            value.value = value.min_value;
        } else {
            value.value = value.value - value.increments;
        }
        option->value.threshold = value;
        break;
    }
    case TIMER: {
        t_arr_index index = option->value.timer;
        if (index <= 0) {
            option->value.timer = 0;

        }else{
            option->value.timer--;
        }
        break;
    }
    }
    return 1;
}

/*
for the timer it should be different
2ms -> 10ms -> 100ms -> 500ms -> 1s ->2s -> 5s -> 10s -> 20s -> 30s-> 1m -> 2m
-> 5m -> 10m ->20m -> 30m -> 1h ->2h possible approaches:
- function that computes these on the fly
- static array
*/

int32_t timer_option_get_index(char *str) {
    int i;
    for (i = 0; i < TIMER_OPTION_ARR_LEN; i++) {
        if (!strcmp(str, timer_option_arr[i].name)) {
            return i;
        }
    }
    return -1;
}
int32_t timer_option_get_value(t_arr_index index) {
    if (index >= 0 && index < TIMER_OPTION_ARR_LEN) {
        return timer_option_arr[index].value;
    }
    return -1;
}
char *timer_option_get_name(t_arr_index index) {
    if (index >= 0 && index < TIMER_OPTION_ARR_LEN) {
        return timer_option_arr[index].name;
    }
    return OUT_OF_RANGE;
}


void to_string_switch_default(char * buf, int32_t value,size_t buf_len){
    if(value){
        snprintf(buf,buf_len,"ON");
    }else{
        snprintf(buf,buf_len,"OFF");
    }
}
void to_string_threshold_default(char * buf, int32_t value,size_t buf_len){
    snprintf(buf,buf_len, "%d u", value);
}
void to_string_timer_default(char * buf, int32_t value,size_t buf_len){
    snprintf(buf,buf_len,"%.*s",TIMER_AMOUNT_NAME_LEN + 1,timer_option_get_name(value));
    
}
