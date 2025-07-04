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


int activate_pump1_index, activate_pump2_index, deactivate_pump1_index, deactivate_pump2_index;
bool act_1, act_2;

void pump_init();
void activatePump1();
void activatePump2();
void turnOffPump1();
void turnOffPump2();
#endif
