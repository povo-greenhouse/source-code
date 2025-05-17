#include "msp.h"
#include "scheduling/scheduler.h"
#include "scheduling/timer.h"
#include "scheduling/scheduling_test.h"
#include "option_menu/option_menu.h"
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
}
