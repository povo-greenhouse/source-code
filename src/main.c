#include "msp.h"
#include "include/scheduling/scheduler.h"
#include "include/scheduling/timer.h"
#include "test/scheduling_test.h"
#include "include/option_menu/option_menu.h"
/**
 * main.c
 */
void main(void)
{
	WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD;		// stop watchdog timer

	scheduler_init();
	timer_init();
	
	/*
	scheduler blink test
	 */
	blink_test_init();

	while(1){
	    __sleep();
	}
}
