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
#include <ti/devices/msp432p4xx/driverlib/driverlib.h>
#include <ti/grlib/grlib.h>

#define MAX_OPTIONS 40
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

/*
    The global option list for the option menu
*/
OptionList option_list;



bool option_menu_is_enabled;
/*
    initializes the option list
    to be called in main
 */
void option_menu_init_option_list();
/*
    pushes an option in the list
    arguments:
    - option: the option to be pushed
    returns:
    - position of the option if it has been succesfully pushed
    - -1 if the list is full

 */
int32_t option_menu_push_option(Option option);
/*

    pops an option from the list
    returns:
    - 1 if it has been removed succesfully
    - -1 if the list's empty
 */
int32_t option_menu_pop_option();




/*
    index of the current setting pointed to in the menu
 */
int32_t current_setting;

/*
    make the current setting pointer go to the next one.
    returns:
    - -1 if current_setting reached the end of the list
    - current_setting index
*/
int option_menu_nav_next_option();
/*
    make the current setting pointer go to the previous one
    returns:
    - -1 if current setting reached the end of the lsit
    - current_setting index
 */
int option_menu_nav_prev_option();



/*
    toggles the option menu on or off
    when it's toggled on
    - the screen printing and input routines are enabled
    when it's toggled off
 */
void option_menu_toggle();

void option_menu_init(Graphics_Context * graphics_context);
#endif /* OPTION_MENU_OPTION_MENU_H_ */
