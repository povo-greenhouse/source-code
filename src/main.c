
#include "msp.h"
#include "include/scheduling/scheduler.h"
#include "include/scheduling/timer.h"
#include "test/scheduling_test.h"
#include "include/option_menu/option_menu.h"
#include <ti/devices/msp432p4xx/driverlib/driverlib.h>
#include <ti/grlib/grlib.h>
#include "include/LcdDriver/Crystalfontz128x128_ST7735.h"


/* Graphic library context */
Graphics_Context g_sContext;

void _graphicsInit()
{
    /* Initializes display */
    Crystalfontz128x128_Init();

    /* Set default screen orientation */
    Crystalfontz128x128_SetOrientation(LCD_ORIENTATION_UP);

    /* Initializes graphics context */
    Graphics_initContext(&g_sContext, &g_sCrystalfontz128x128, &g_sCrystalfontz128x128_funcs);
        Graphics_setForegroundColor(&g_sContext, GRAPHICS_COLOR_RED);
        Graphics_setBackgroundColor(&g_sContext, GRAPHICS_COLOR_WHITE);
        GrContextFontSet(&g_sContext, &g_sFontFixed6x8);
        Graphics_clearDisplay(&g_sContext);
        /*
        Graphics_drawStringCentered(&g_sContext,
                                        "Joystick:",
                                        AUTO_STRING_LENGTH,
                                        64,
                                        30,
                                        OPAQUE_TEXT);
        */
}
void main(void)
{
	WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD;		// stop watchdog timer
	_graphicsInit();
	scheduler_init();
	timer_init();
	init_option_menu(&g_sContext);
	

	//blink_test_init();
	option_menu_test_main();
	while(1){
	    if(scheduler_state ==AWAKE){
	        scheduler();
	    }
	    __WFI();
	}
}
