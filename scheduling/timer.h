/*
 * timer.h
 *
 *  Created on: May 11, 2025
 *      Author: riginel
 */

#ifndef TIMER_H_
#define TIMER_H_


#include "msp.h"
#include "scheduling/scheduler.h"
#include "stdint.h"


//timer period in milliseconds
#define TIMER_PERIOD 2

/*
    utility function that computes the countdown value for the timer.
    arguments:
    - period: the timer's period in milliseconds
    - divider: the prescaler's division factor

 */
inline int32_t compute_countdown(int32_t period,int32_t divider);


void timer_init();



void TA0_0_IRQHandler();
#endif /* TIMER_H_ */
