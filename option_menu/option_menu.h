/*
 * option_menu.h
 *
 *  Created on: May 15, 2025
 *      Author: riginel
 */

#ifndef OPTION_MENU_OPTION_MENU_H_
#define OPTION_MENU_OPTION_MENU_H_
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include "options.h"


#define MAX_OPTIONS 20
/*
    struct representing the list of options
    fields:
    - arr: containing options
    - len: number of options present in the array
 */
typedef struct {
    Option arr[MAX_OPTIONS];
    int32_t len;
}OptionList;


OptionList option_list;

int32_t current_setting;
/*
    initializes the option list
    to be called in main
 */
void init_option_list();
/*
    pushes an option in the list
    arguments:
    - option: the option to be pushed
    returns:
    - position of the option if it has been succesfully pushed
    - -1 if the list is full

 */
int32_t push_option(Option option);
/*

    pops an option from the list
    returns:
    - 1 if it has been removed succesfully
    - -1 if the list's empty
 */
int32_t pop_option();

/*
    make the current setting pointer go to the next one.
    returns:
    - -1 if current_setting reached the end of the list
    - current_setting index
*/
int nav_next_option();
/*
    make the current setting pointer go to the previous one
    returns:
    - -1 if current setting reached the end of the lsit
    - current_setting index
 */
int nav_prev_option();




#endif /* OPTION_MENU_OPTION_MENU_H_ */
