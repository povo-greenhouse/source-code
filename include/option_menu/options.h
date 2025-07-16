

#ifndef OPTIONS_H_
#define OPTIONS_H_

#include "stdbool.h"
#include "stdint.h"
#include "string.h"





/*
    three types of options
    - THRESHOLD: threshold for a sensor
    - TIMER: time between routine schedules
    - SWITCH: ON/OFF
 */
typedef enum { THRESHOLD, TIMER, SWITCH } OptionType;
/*
 * ------------------------------------------------------------
 *                      THRESHOLD Option
 * ------------------------------------------------------------
 */
/*
    struct representing a threshold option
    fields:
    - value: the value of the threshold
    - min_value: the minimum value the threshold can be
    - max_value: the maximum value the threshold can be
    - increments: how much the value shall increment or decrement every change
*/
typedef struct {
  int32_t value;
  int32_t min_value;
  int32_t max_value;
  int32_t increments;
} OptionTU;

/*
 * ------------------------------------------------------------
 *                      TIMER OPTION
 * ------------------------------------------------------------
 */

#define TIMER_AMOUNT_NAME_LEN 6
#define TIMER_OPTION_ARR_LEN 18
/*
    struct representing the timer option
    this sits inside the array defined before
    fields:
    - name: the name of the time interval
    - amount: amount in milliseconds
    example: name: "1s", value: 1000

*/
typedef struct {
    char name[TIMER_AMOUNT_NAME_LEN];
    int32_t value;
} TimerAmount;


// array with the timer options. accessed through an index
extern TimerAmount timer_option_arr[TIMER_OPTION_ARR_LEN];

//index for timer arr
typedef  int32_t t_arr_index;

/*
    function that returns the index of a timer option from t_arr_index given it's corresponding string
    arguments:
    -str: the string e.g. "2s"
    returns:
    - the index if it corresponds to an entry in the array
    - -1 if it's not an entry

 */
int32_t timer_option_get_index(char * str);
/*
    get value in milliseconds using index
    arguments:
    - index: the index in the array
    returns:
    - value in millis if the index is valid
    - -1 otherwise
 */
int32_t timer_option_get_value(t_arr_index index);
/*
    get the name using index
    arguments:
    - index: the index in the array
    returns:
    - pointer to the string
    - -1 otherwise
 */
char * timer_option_get_name(t_arr_index index);

/*
 * ------------------------------------------------------------
 *                      OPTION UNION
 * ------------------------------------------------------------
 */

/*

    union representing the three different option types
    possible values:
    - threshold: in case it's a threshold, using a threshold option struct
    - timer: in case it's a timer, using a index to the threshold array
    - sswitch: in case it's a switch, using a bool
 */
typedef union {
  OptionTU threshold;
  t_arr_index timer;
  bool sswitch;
} OptionUnion;
/*
    function to create a new option union with a threshold
    arguments:
    - value, min_value,max_value,increments: same fields as in OptionTU
    - error: pointer to an int changed in case of error, for instance min_value bigger than max_value
    returns:
    new option union with the specified values


 */
OptionUnion option_u_new_threshold(int32_t value, int32_t min_value,
                                   int32_t max_value, int32_t increments,
                                   int *error);
/*
    function to create a new option union with a timer
    arguments:
    - str: the string representing the time amount
    - error: pointer to an int changed in case of error
    returns:
    new option union with the specified values
*/

OptionUnion option_u_new_timer( char *str,
                               int *error);
/*
    function to create a new option union with a switch
    arguments:
    - sswitch: the switch value
 */

OptionUnion option_u_new_switch(bool sswitch);


/*
 * ------------------------------------------------------------
 *                      OPTION UNION
 * ------------------------------------------------------------
 */

/*
    type representing a pointer to a function to print the value of the option
    arguments:
    - pointer to buffer where we should write
    - the value
    - size of the buffer
    default versions are available below
*/
typedef void (*FmtStringFP)(char *,int32_t,size_t);


/*
    pointer to a function that activates when an option is changed in the menu
*/
typedef void (*OnChangeFP)(int32_t);

#define OPTION_NAME_MAX_LENGTH 50

/*
    struct representing an option in the menu
    fields:
    - name: the name of the option
    - type: the option type
    - value: the value union
    - on_change_action: function called when the value is changed(in order to update fields etc)
    - to_string: function called when we want the stringified version of the option's value
 */

typedef struct {
  char name[OPTION_NAME_MAX_LENGTH + 1];
  OptionType type;
  OptionUnion value;
  OnChangeFP on_change_action;
  FmtStringFP to_string;
  bool changed;
} Option;

int32_t option_get_value(Option * option);

Option option_new(char *name, OptionType type, OptionUnion value,
                  OnChangeFP action,FmtStringFP to_string);


/*
    default to string functions
*/

void to_string_switch_default(char * buf, int32_t value,size_t buf_len);
void to_string_threshold_default(char * buf, int32_t value,size_t buf_len);
void to_string_timer_default(char * buf, int32_t value,size_t buf_len);
void to_string_manual_auto(char * buf, int32_t value, size_t buf_len);
/*
    functions to increment and decrement the option values
    the mechanism depends on the option below:
    - threshold: value is increased/decreased by increment until it reaches max value/min value
    - timer: value pointer is increased/decreased
    - switch: on/off
*/

int32_t option_increment(Option *option);
int32_t option_decrement(Option *option);





#endif
