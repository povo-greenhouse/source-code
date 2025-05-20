/*
 * timer.c
 *
 *  Created on: May 11, 2025
 *      Author: riginel
 */
#include "timer.h"  // is this header file from the ti library or your own code?
#include "msp.h"
#include "include/scheduling/timer.h"

inline int compute_countdown(int32_t period, int32_t divider) {
    return (32768 / divider) / (1000 / period);
}

void timer_init() {
    /*
    TIMER_A0->CCTL[0] = TIMER_A_CCTLN_CCIE;

    TIMER_A0->CCR[0] = compute_countdown(TIMER_PERIOD, 8);
    // configure clock source of timer
    TIMER_A0->CTL = TIMER_A_CTL_SSEL__SMCLK;
        // set operation mode
    TIMER_A0->CTL |= TIMER_A_CTL_MC__CONTINUOUS;
   // use a prescaler to divide
    TIMER_A0->CTL |= TIMER_A_CTL_ID_3;


    // enable overflow interrupt
    TIMER_A0->CTL |= TIMER_A_CTL_IE;
    */
    TIMER_A0->CTL = TIMER_A_CTL_SSEL__ACLK;
    TIMER_A0->CTL |= TIMER_A_CTL_MC__UP;
    TIMER_A0->CCR[0] = compute_countdown(2,1);
    TIMER_A0 -> CCTL[0] = TIMER_A_CCTLN_CCIE;

    // enable irq9 line for the timer interrupt to catch overflow
    // NVIC->ISER[0] = 1 << ((TA0_0_IRQn) & 31);
    enable_interrupt();
}
void enable_interrupt() { NVIC->ISER[0] = 1 << ((TA0_0_IRQn) & 31); }
void disable_interrupt() { NVIC->ISER[0] = 0 << ((TA0_0_IRQn) & 31); }
void TA0_0_IRQHandler() {

    TIMER_A0->CCTL[0] &= ~TIMER_A_CCTLN_CCIFG;
    TIMER_A0->CTL &= ~TIMER_A_CTL_IFG;
    timer_interrupt(TIMER_PERIOD);
}
