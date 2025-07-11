/*
 * input.h
 *
 *  Created on: May 22, 2025
 *      Author: riginel
 */

#ifndef INCLUDE_OPTION_MENU_OPTION_MENU_INPUT_H_
#define INCLUDE_OPTION_MENU_OPTION_MENU_INPUT_H_
#include<string.h>
/*
    the possible inputs for the option menu
 */
typedef enum {
    NONE,
    UP,
    DOWN,
    LEFT,
    RIGHT,
    BUTTON_A,
    BUTTON_B,
    JOYSTICK_SELECT
} ControllerInputOption;
ControllerInputOption option_input_from_str(char * buf, uint16_t len);
#define INPUT_QUEUE_CAPACITY 50
/*
    scheduler's task queue, as a circular buffer
    fields:
    - arr: underlying array
    - write_index: index where to write
    - read_index: index where to read
*/
typedef struct {
    ControllerInputOption arr[INPUT_QUEUE_CAPACITY];
    int write_index;
    int read_index;
}InputQueue;

//global input queue
InputQueue input_buffer;


/*
    enqueues the last task, returning it
 */
int input_buffer_enqueue(ControllerInputOption input);
//dequeues the last task, returning it
ControllerInputOption input_buffer_dequeue();



void init_option_menu_input(void);

#endif /* INCLUDE_OPTION_MENU_OPTION_MENU_INPUT_H_ */
