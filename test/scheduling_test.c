/*
 * scheduling_test.c
 *
 *  Created on: May 12, 2025
 *      Author: riginel
 */
#include "scheduling_test.h"
#include "scheduling/scheduler.h"
#include "msp.h"
void sched_test_configurePorts(){
    /* configure P2.0, P2.1 and P2.2 as GPIO */
    P2->SEL0 &= ~(BIT0| BIT1 | BIT2);
    P2->SEL1 &= ~(BIT0| BIT1 | BIT2);

    /* set Port 2.0, 2.1 and 2.2 as output */
    P2->DIR |= (BIT0 | BIT1 | BIT2);

    /* clear the pins */
    P2->OUT &= ~(BIT0| BIT1 | BIT2);
}


void blink_routine(){
    //toggle led
    P2->OUT ^= BIT0;

}
void blink_routine_b(){
    P2->OUT ^= BIT1;
}

void blink_test_init(){
    sched_test_configurePorts();
    //create the task
    STask task =  {
        blink_routine,
        1000,
        1000,
        true
    };
    STask task2 = {
       blink_routine_b,
       2000,
       2000,
       true
    };
    push_task(task);
    push_task(task2);

    //now it should schedule the blink every second

}
