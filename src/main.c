
#include "msp.h"
#include "include/scheduling/scheduler.h"
#include "include/scheduling/timer.h"
#include "test/scheduling_test.h"
#include "include/option_menu/option_menu.h"
#include <ti/devices/msp432p4xx/driverlib/driverlib.h>
#include <ti/grlib/grlib.h>
#include "include/LcdDriver/Crystalfontz128x128_ST7735.h"

void main(void)
{
	WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD;		// stop watchdog timer

	scheduler_init();
	timer_init();
	init_option_menu(NULL);
	

	//blink_test_init();
	option_menu_test_main();
	while(1){
	    __sleep();
	}
}
