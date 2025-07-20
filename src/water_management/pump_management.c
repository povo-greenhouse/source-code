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

// Hardware pin definitions
// P4.7 - Pump 1 control pin
// P4.6 - Pump 2 control pin
// P5.4 - Sensor 1 (water level sensor)
// P5.5 - Sensor 2 (water level sensor)
#define PUMP1_PORT P4
#define PUMP2_PORT P4
#define PUMP1_PIN BIT7
#define PUMP2_PIN BIT6

/**
 * pump_init() - Initialize both water pumps
 * 
 * Configures GPIO pins for pump control and ensures both pumps start in OFF state.
 * This function must be called before any pump operations.
 */
void pump_init() {
#ifndef DEBUG
    printf("Executing: pump_init()\n");
#endif
    
    // Configure Pump 1 (P4.7)
    PUMP1_PORT->DIR |= PUMP1_PIN;     // Set pin direction to output
    PUMP1_PORT->OUT &= ~PUMP1_PIN;    // Initialize to LOW (pump OFF)

    // Configure Pump 2 (P4.6)
    PUMP2_PORT->DIR |= PUMP2_PIN;     // Set pin direction to output
    PUMP2_PORT->OUT &= ~PUMP2_PIN;    // Initialize to LOW (pump OFF)
}

/**
 * activate_pump1() - Turn on water pump 1
 * 
 * Activates pump 1 if not blocked by safety mechanisms.
 * Updates task scheduler to manage pump timing and sends status to communication interface.
 */
void activate_pump1() {
    // Safety check: only activate if not blocked
    if (!block) {
        #ifndef DEBUG
        printf("Executing: activatePump1()\n");
        #endif
        
        // Send pump activation status to communication interface
        send_data(6, 1, 0);
        
        // Turn on pump 1 (set pin HIGH)
        PUMP1_PORT->OUT |= PUMP1_PIN;

        // Update task scheduler: disable activation task, enable deactivation task
        disable_task_at(activate_pump1_index);
        enable_task_at(deactivate_pump1_index);
        
        // Update pump status flag
        act_1 = true;
    } else {
        #ifndef DEBUG
        printf("Blocked pump 1 activation\n");
        #endif
    }
}


/**
 * turn_off_pump1() - Turn off water pump 1
 * 
 * Deactivates pump 1 if not blocked by safety mechanisms.
 * Updates task scheduler and sends status to communication interface.
 */
void turn_off_pump1() {
    // Safety check: only deactivate if not blocked
    if (!block) {
        #ifndef DEBUG
        printf("Executing: turnOffPump1()\n");
        #endif
        
        // Send pump deactivation status to communication interface
        send_data(6, 0, 0);

        // Turn off pump 1 (set pin LOW)
        PUMP1_PORT->OUT &= ~PUMP1_PIN;

        // Update task scheduler: disable deactivation task, enable activation task
        disable_task_at(deactivate_pump1_index);
        enable_task_at(activate_pump1_index);
        
        // Update pump status flag
        act_1 = false;
    } else {
        #ifndef DEBUG
        printf("Blocked pump 1 deactivation\n");
        #endif
    }
}

/**
 * activate_pump2() - Turn on water pump 2
 * 
 * Activates pump 2 and updates task scheduler.
 * Note: No block check implemented for pump 2 (consider adding for consistency).
 */
void activate_pump2() {
    #ifndef DEBUG
    printf("Executing: activatePump2()\n");
    #endif
    
    // Send pump activation status to communication interface
    send_data(7, 1, 0);
    
    // Turn on pump 2 (set pin HIGH)
    PUMP2_PORT->OUT |= PUMP2_PIN;

    // Update task scheduler: disable activation task, enable deactivation task
    disable_task_at(activate_pump2_index);
    enable_task_at(deactivate_pump2_index);
    
    // Update pump status flag
    act_2 = true;
}

/**
 * turn_off_pump2() - Turn off water pump 2
 * 
 * Deactivates pump 2 if not blocked by safety mechanisms.
 * Updates task scheduler and sends status to communication interface.
 */
void turn_off_pump2() {
    // Safety check: only deactivate if not blocked
    if (!block) {
        #ifndef DEBUG
        printf("Executing: turnOffPump2()\n");
        #endif
        
        // Send pump deactivation status to communication interface
        send_data(7, 0, 0);
        
        // Turn off pump 2 (set pin LOW)
        PUMP2_PORT->OUT &= ~PUMP2_PIN;
        
        // Update task scheduler: disable deactivation task, enable activation task
        disable_task_at(deactivate_pump2_index);
        enable_task_at(activate_pump2_index);
        
        // Update pump status flag
        act_2 = false;
    } else {
        #ifndef DEBUG
        printf("Blocked pump 2 deactivation\n");
        #endif
    }
}

/**
 * add_pump_options() - Initialize pump configuration options for the menu system
 * 
 * Creates menu options for manual/automatic mode control and timing configuration.
 * Allows users to control pump behavior through the interface.
 */
void add_pump_options() {
    int error;
    
    // Manual/Automatic mode toggle
    OptionUnion manual_mode = option_u_new_switch(false);
    option_menu_push_option(option_new("Pumps Manual Mode", SWITCH, manual_mode, 
                                      upd_manual_mode, to_string_manual_auto));

    // Manual control switches for individual pumps
    OptionUnion pump1_toggled = option_u_new_switch(false);
    option_menu_push_option(option_new("Pump 1 Status", SWITCH, pump1_toggled, 
                                      upd_manual_pump1_status, to_string_switch_default));

    OptionUnion pump2_toggled = option_u_new_switch(false);
    option_menu_push_option(option_new("Pump 2 Status", SWITCH, pump2_toggled, 
                                      upd_manual_pump2_status, to_string_switch_default));

    // Timing configuration options
    OptionUnion enable_pump1_time = option_u_new_timer(
        timer_option_get_name_by_value(PUMP1_ENABLE_TIME_DEFAULT), &error);
    option_menu_push_option(option_new("Pump 1 Enable Time", TIMER, enable_pump1_time, 
                                      upd_pump1_enable_time, to_string_timer_default));

    OptionUnion enable_pump2_time = option_u_new_timer(
        timer_option_get_name_by_value(PUMP2_ENABLE_TIME_DEFAULT), &error);
    option_menu_push_option(option_new("Pump 2 Enable Time", TIMER, enable_pump2_time, 
                                      upd_pump2_enable_time, to_string_timer_default));

    OptionUnion disable_pump1_time = option_u_new_timer(
        timer_option_get_name_by_value(PUMP1_DISABLE_TIME_DEFAULT), &error);
    option_menu_push_option(option_new("Pump 1 Disable Time", TIMER, disable_pump1_time, 
                                      upd_pump1_disable_time, to_string_timer_default));

    OptionUnion disable_pump2_time = option_u_new_timer(
        timer_option_get_name_by_value(PUMP2_DISABLE_TIME_DEFAULT), &error);
    option_menu_push_option(option_new("Pump 2 Disable Time", TIMER, disable_pump2_time, 
                                      upd_pump2_disable_time, to_string_timer_default));
}

/**
 * Timer update functions - Update pump timing configurations
 * These functions modify both the option values and the task scheduler timings
 */

void upd_pump1_enable_time(int32_t val) {
    water_option_values.enable_pump1_time = val;
    task_list.task_array[activate_pump1_index].max_time = val;
}

void upd_pump2_enable_time(int32_t val) {
    water_option_values.enable_pump2_time = val;
    task_list.task_array[activate_pump2_index].max_time = val;
}

void upd_pump1_disable_time(int32_t val) {
    water_option_values.disable_pump1_time = val;
    task_list.task_array[deactivate_pump1_index].max_time = val;
}

void upd_pump2_disable_time(int32_t val) {
    water_option_values.disable_pump2_time = val;
    task_list.task_array[deactivate_pump2_index].max_time = val;
}

/**
 * upd_manual_mode() - Switch between manual and automatic pump control
 * @val: 1 for manual mode, 0 for automatic mode
 * 
 * In manual mode: Disables all automatic tasks and turns off pumps
 * In automatic mode: Re-enables pump 1 automatic control
 */
void upd_manual_mode(int32_t val) {
    // Update the manual mode status
    water_option_values.manual_mode = val;
    
    if (val) { // MANUAL MODE
        // Disable all automatic pump tasks
        disable_task_at(activate_pump1_index);
        disable_task_at(activate_pump2_index);
        disable_task_at(deactivate_pump1_index);
        disable_task_at(deactivate_pump2_index);
        
        // Turn off both pumps for safety
        PUMP1_PORT->OUT &= ~PUMP1_PIN;
        PUMP2_PORT->OUT &= ~PUMP2_PIN;
    } else { // AUTOMATIC MODE
        // Reactivate pump 1 automatic control
        enable_task_at(activate_pump1_index);
        // Note: Consider also enabling pump 2 automatic control
    }
}

/**
 * upd_manual_pump1_status() - Manual control for pump 1
 * @val: 1 to turn on pump, 0 to turn off pump
 * 
 * Only affects pump operation when in manual mode.
 */
void upd_manual_pump1_status(int32_t val) {
    water_option_values.pump1_enabled = val;
    
    // Only control pump if in manual mode
    if (water_option_values.manual_mode) {
        if (val) {
            // Turn on pump 1
            PUMP1_PORT->OUT |= PUMP1_PIN;
        } else {
            // Turn off pump 1
            PUMP1_PORT->OUT &= ~PUMP1_PIN;
        }
    }
}

/**
 * upd_manual_pump2_status() - Manual control for pump 2
 * @val: 1 to turn on pump, 0 to turn off pump
 * 
 * Only affects pump operation when in manual mode.
 */
void upd_manual_pump2_status(int32_t val) {
    water_option_values.pump2_enabled = val;
    
    // Only control pump if in manual mode
    if (water_option_values.manual_mode) {
        if (val) {
            // Turn on pump 2
            PUMP2_PORT->OUT |= PUMP2_PIN;
        } else {
            // Turn off pump 2
            PUMP2_PORT->OUT &= ~PUMP2_PIN;
        }
    }
}

