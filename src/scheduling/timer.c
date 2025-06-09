/*
 * timer.c
 *
 *  Created on: May 11, 2025
 *      Author: riginel
 */

#include "msp.h"
#include "include/scheduling/timer.h"

inline int compute_countdown(int32_t period, int32_t divider) {
    return (3000000 / divider) / (1000 / period);
}

void timer_init() {
    
    TIMER_A0->CTL = TIMER_A_CTL_SSEL__SMCLK;
    TIMER_A0->CTL |= TIMER_A_CTL_MC__UP;
    TIMER_A0->CTL |= TIMER_A_CTL_ID_3;
    TIMER_A0->CCR[0] = compute_countdown(TIMER_PERIOD,8);
    TIMER_A0 -> CCTL[0] = TIMER_A_CCTLN_CCIE;

    // enable irq9 line for the timer interrupt to catch overflow
    // NVIC->ISER[0] = 1 << ((TA0_0_IRQn) & 31);
    enable_timer_interrupt();
}
void enable_timer_interrupt() { NVIC->ISER[0] = 1 << ((TA0_0_IRQn) & 31); }
void disable_timer_interrupt() { NVIC->ISER[0] = 0 << ((TA0_0_IRQn) & 31); }
void TA0_0_IRQHandler() {

    TIMER_A0->CCTL[0] &= ~TIMER_A_CCTLN_CCIFG;
    TIMER_A0->CTL &= ~TIMER_A_CTL_IFG;
    timer_interrupt(TIMER_PERIOD);
}
