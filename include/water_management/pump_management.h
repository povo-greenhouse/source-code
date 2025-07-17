/*
 *
 * PumpManagement.h
 *
 *  Created on: 24 mag 2025
 *      Author: vincenzo
 */
#include <stdbool.h>
#include "IOT/IOT_communication.h"
#include "scheduling/scheduler.h"
#include "msp.h"
#ifndef PUMPMANAGEMENT_H_
#define PUMPMANAGEMENT_H_

#define PUMP1_ENABLE_TIME_DEFAULT 10000
#define PUMP2_ENABLE_TIME_DEFAULT 10000
#define PUMP1_DISABLE_TIME_DEFAULT 20000
#define PUMP2_DISABLE_TIME_DEFAULT 20000

int activate_pump1_index, activate_pump2_index, deactivate_pump1_index, deactivate_pump2_index;
bool act_1, act_2;


void pump_init();
void activate_pump1();
void activate_pump2();
void turn_off_pump1();
void turn_off_pump2();

void add_pump_options();
#endif
