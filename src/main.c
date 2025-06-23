#include "msp.h"
#include "environment_systems/temperature.h"
#include "light_system/growing_light.h"
#include "environment_systems/air_quality.h"
#include "environment_systems/buzzer_utils.h"
#include "scheduling/scheduler.h"
#include "scheduling/timer.h"
#include "../test/scheduling_test.h"
#include "../test/option_menu_test.h"
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

void add_tasks_to_option_menu(){

    int err;

    // Option option_new(char *name,
                    //    OptionType type,
                    //    OptionUnion value,
                    //    OnChangeFP action,
                    //    FmtStringFP to_string)

    /*
     * options for light system
     */

    // power to grow lights
    OptionUnion opt_led_s_power = option_u_new_switch(false);
    Option leds_switch = option_new("power leds", SWITCH, opt_led_s_power, power_on_or_off, to_string_switch_default);
    option_menu_push_option(leds_switch);

    // switch for the user to change between manual and automatic modes
    OptionUnion opt_led_s_mode = option_u_new_switch(false);
    Option leds_manual = option_new("manual<->automatic", SWITCH, opt_led_s_mode, grow_light_set_mode, to_string_switch_default);
    option_menu_push_option(leds_manual);

    // timer to update light sensor values
    OptionUnion opt_led_t = option_u_new_timer("500ms", &err);
    if(err == 1){
        #ifdef DEBUG
                printf("500ms is not on the list of possible timing values\n");
        #endif
        return;
    }
    Option leds_timer = option_new("timer to light", TIMER, opt_led_t, update_light_timer, to_string_timer_default);
    option_menu_push_option(leds_timer);

    // light brightness threshold
    OptionUnion opt_led_thr = option_u_new_threshold(grow_light_get_threshold(), MIN_BRIGHTNESS, MAX_BRIGHTNESS, 20, &err);
    if(err == 1){
        #ifdef DEBUG
                printf("Current threshold exeeds the range\n");
        #endif
        return;
    }

    Option leds_threshold = option_new("change light threshold", THRESHOLD, opt_led_thr, grow_light_set_threshold, to_string_threshold_default);
    option_menu_push_option(leds_threshold);

    /*
     * options for temperature system (does not include activation of the buzzer)
     */

    // temperature variable function (not including buzzer)
    OptionUnion opt_temp_t = option_u_new_timer("5s", &err);
    if(err == 1){
        #ifdef DEBUG
                printf("5s is not on the list of possible timing values\n");
        #endif
        return;
    }
    Option temp_timer = option_new("timer to temp sensor", TIMER, opt_temp_t, update_temperature_timer, to_string_timer_default);
    option_menu_push_option(temp_timer);

    // higher temperature threshold
    OptionUnion opt_temp_higher_thr = option_u_new_threshold(temp_get_higher_threshold(), 0, 42, 5, &err);
    if(err == 1){
        #ifdef DEBUG
                printf("Current threshold exeeds the range\n");
        #endif
        return;
    }
    Option temp_higher_threshold = option_new("change temp higher threshold val", THRESHOLD, opt_temp_higher_thr, temp_set_higher_threshold, to_string_threshold_default);
    option_menu_push_option(temp_higher_threshold);

    // lower temperature threshold
    OptionUnion opt_temp_lower_thr = option_u_new_threshold(temp_get_lower_threshold(), 0, 42, 5, &err);
    if(err == 1){
        #ifdef DEBUG
                printf("Current threshold exeeds the range\n");
        #endif
        return;
    }
    Option temp_lower_threshold = option_new("change temp lower threshold val", THRESHOLD, opt_temp_lower_thr, temp_set_lower_threshold, to_string_threshold_default);
    option_menu_push_option(temp_lower_threshold);

    /*
     * options for air system (does not include activation of the buzzer)
     */

    // timer to update air sensor values
    OptionUnion opt_air_t = option_u_new_timer("2s", &err);
    if(err == 1){
        #ifdef DEBUG
                printf("Current threshold exeeds the range\n");
        #endif
        return;
    }
    Option air_timer = option_new("timer to air qual", TIMER, opt_air_t, update_air_timer, to_string_timer_default);
    option_menu_push_option(air_timer);


    // air quality threshold
    OptionUnion opt_air_thr = option_u_new_threshold(air_get_threshold(), 0, 50, 5, &err);
    if(err == 1){
        #ifdef DEBUG
                printf("Current threshold exeeds the range\n");
        #endif
        return;
    }
    Option air_threshold = option_new("change air threshold val", THRESHOLD, opt_air_thr, air_set_threshold, to_string_threshold_default);
    option_menu_push_option(air_threshold);

    /*
     * options for water system
     */

    return;

}

void initialize_system(){
    _graphicsInit();
    scheduler_init();
    timer_init();
    option_menu_init(&g_sContext);
    add_tasks_to_option_menu();

    I2C_init();
    Init_I2C_GPIO();
    init_buzzer();
    grow_light_init();
    temp_sensor_init();
    air_init();
}

void main(void){

    WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD;     // stop watchdog timer

    initialize_system();


    while(1){
       if(scheduler_state == AWAKE){
            scheduler();
        }
        __WFI();
    }
}
