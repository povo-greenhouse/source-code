#include "msp.h"
#include "environment_systems/temperature.h"
#include "light_system/growing_light.h"
#include "scheduling/scheduler.h"
#include "scheduling/timer.h"
#include "../test/scheduling_test.h"
#include "option_menu/option_menu.h"
#include <stdio.h>
#include <ti/devices/msp432p4xx/driverlib/driverlib.h>
#include <ti/grlib/grlib.h>

#include "../lib/HAL_I2C.h"
#include "../include/LcdDriver/Crystalfontz128x128_ST7735.h"

Graphics_Context g_sContext;

void _graphicsInit()
{
    Crystalfontz128x128_Init();

    Crystalfontz128x128_SetOrientation(LCD_ORIENTATION_UP);

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

void initialize_system(){
    _graphicsInit();
    scheduler_init();
    timer_init();
//    init_option_menu(&g_sContext);

    I2C_init();
    Init_I2C_GPIO();
    grow_light_init();
    temp_sensor_init();
}

//void main(void){
//    WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD;     // stop watchdog timer
//
//    initialize_system();
//
//    while(1){
//        if(scheduler_state == AWAKE){
//            scheduler();
//        }
//        __WFI();
//    }
//    Interrupt_disableMaster();
//
//    PCM_setCoreVoltageLevel(PCM_VCORE1);
//
//    FlashCtl_setWaitState(FLASH_BANK0, 2);
//    FlashCtl_setWaitState(FLASH_BANK1, 2);
//
//    CS_setDCOCenteredFrequency(CS_DCO_FREQUENCY_3);
//    CS_initClockSignal(CS_MCLK, CS_DCOCLK_SELECT, CS_CLOCK_DIVIDER_1);
//
//    while (1) {
//        update_temperature();
//        update_light();
//        volatile int i=0;
//        for(;i<10000;i++);
//    }
//}
