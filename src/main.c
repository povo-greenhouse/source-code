#include "msp.h"
#include "environment_systems/temperature.h"
#include "light_system/growing_light.h"
#include "environment_systems/air_quality.h"
#include "environment_systems/buzzer.h"
#include "scheduling/scheduler.h"
#include "scheduling/timer.h"
#include "../test/scheduling_test.h"
#include "option_menu/option_menu.h"
#include "option_menu/options.h"
#include "uart_communication/uart_comm.h"
#include "water_management/water_init.h"
#include "water_management/water_reading.h"
#include "water_management/pump_management.h"
#include "adc/adc.h"

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

    // Power to grow lights
    OptionUnion opt_led_s_power = option_u_new_switch(false);
    Option leds_switch = option_new("power leds", SWITCH, opt_led_s_power, power_on_or_off, to_string_switch_default);
    option_menu_push_option(leds_switch);

    // switch for the user to change between manual and automatic modes for the grow lights
    OptionUnion opt_led_s_mode = option_u_new_switch(false);
    Option leds_manual = option_new("light mode", SWITCH, opt_led_s_mode, grow_light_set_mode, to_string_manual_auto);
    option_menu_push_option(leds_manual);

    // Timer to update light sensor values
    OptionUnion opt_led_t = option_u_new_timer("10s", &err);
    if(err == 1){
        #ifdef DEBUG
                puts("10s is not on the list of possible timing values\n");
        #endif
        return;
    }
    Option leds_timer = option_new("light timer", TIMER, opt_led_t, update_light_timer, to_string_timer_default);
    option_menu_push_option(leds_timer);

    // light brightness threshold
    OptionUnion opt_led_thr = option_u_new_threshold(grow_light_get_threshold(), MIN_BRIGHTNESS, MAX_BRIGHTNESS, 20, &err);
    if(err == 1){
        #ifdef DEBUG
                puts("Current threshold exeeds the range\n");
        #endif
        return;
    }

    Option leds_threshold = option_new("change light threshold", THRESHOLD, opt_led_thr, grow_light_set_threshold, to_string_threshold_default);
    option_menu_push_option(leds_threshold);

    /*
     * options for temperature system
     */

    // temperature variable function (not including buzzer)
    OptionUnion opt_temp_t = option_u_new_timer("5s", &err);
    if(err == 1){
        #ifdef DEBUG
                puts("5s is not on the list of possible timing values\n");
        #endif
        return;
    }
    Option temp_timer = option_new("temp sensor timer", TIMER, opt_temp_t, update_temperature_timer, to_string_timer_default);
    option_menu_push_option(temp_timer);

    // higher temperature threshold
    OptionUnion opt_temp_higher_thr = option_u_new_threshold(temp_get_higher_threshold(), 0, 42, 5, &err);
    if(err == 1){
        #ifdef DEBUG
                puts("Current threshold exeeds the range\n");
        #endif
        return;
    }
    Option temp_higher_threshold = option_new("change temp higher threshold", THRESHOLD, opt_temp_higher_thr, temp_set_higher_threshold, to_string_threshold_default);
    option_menu_push_option(temp_higher_threshold);

    // lower temperature threshold
    OptionUnion opt_temp_lower_thr = option_u_new_threshold(temp_get_lower_threshold(), 0, 42, 5, &err);
    if(err == 1){
        #ifdef DEBUG
                puts("Current threshold exeeds the range\n");
        #endif
        return;
    }
    Option temp_lower_threshold = option_new("change temp lower threshold", THRESHOLD, opt_temp_lower_thr, temp_set_lower_threshold, to_string_threshold_default);
    option_menu_push_option(temp_lower_threshold);

    /*
     * options for air system
     */

    // timer to update air sensor values
    OptionUnion opt_air_t = option_u_new_timer("20s", &err);
    if(err == 1){
        #ifdef DEBUG
        puts("20s is not on the list of possible timing values\n");
        #endif
        return;
    }
    Option air_timer = option_new("air system timer", TIMER, opt_air_t, update_air_timer, to_string_timer_default);
    option_menu_push_option(air_timer);


    // air quality threshold
    OptionUnion opt_air_thr = option_u_new_threshold(air_get_threshold(), 0, 50, 5, &err);
    if(err == 1){
        #ifdef DEBUG
                puts("Current threshold exeeds the range\n");
        #endif
        return;
    }
    Option air_threshold = option_new("change air threshold", THRESHOLD, opt_air_thr, air_set_threshold, to_string_threshold_default);
    option_menu_push_option(air_threshold);

    /*
     * options for buzzer
     */
    // Switching buzzer on and off
    OptionUnion opt_buzzer_s_power = option_u_new_switch(false);
    Option buzzer_switch = option_new("power buzzer", SWITCH, opt_buzzer_s_power, manual_power_buzzer, to_string_switch_default);
    option_menu_push_option(leds_switch);

    // switch for the user to change between manual and automatic modes for the buzzer
    OptionUnion opt_buzzer_s_mode = option_u_new_switch(false);
    Option buzzer_manual = option_new("buzzer mode", SWITCH, opt_buzzer_s_mode, set_buzzer_mode, to_string_manual_auto);
    option_menu_push_option(leds_manual);

    /*
     * options for water system
     */
    // todo(add water system options);
    add_water_options();
    return;

}

/// @brief Initializes the hardware components and each system.
/// This function sets up the clock, initializes the graphics context, and configures
/// the various subsystems such as I2C, buzzer, grow light, temperature sensor, and air quality sensor.
/// It also sets up the option menu and adds tasks to it.
void _hwInit(){
    // Halting WDT and disabling master interrupts
    WDT_A_holdTimer();
    Interrupt_disableMaster();

    // Setting DCO to 3MHz
    CS_setDCOCenteredFrequency(CS_DCO_FREQUENCY_3);
    // Setting SMCLK to DCO at 3MHz
    CS_initClockSignal(CS_SMCLK, CS_DCOCLK_SELECT, CS_CLOCK_DIVIDER_1);

    // initializing the greenhouse systems
    _graphicsInit();
    scheduler_init();
    timer_init();
    adc_init();
    option_menu_init(&g_sContext);
    add_tasks_to_option_menu();

    I2C_init();
    Init_I2C_GPIO();
    init_buzzer();
    grow_light_init();
    temp_sensor_init();
    air_init();

    water_init();
    init_GPIOs_IOT();
    uart_init();
    // enabling interrupts
    Interrupt_enableInterrupt(INT_TA1_N);
    Interrupt_enableMaster();

}
void main(void){

    WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD;     // stop watchdog timer

    _hwInit();


    while(1){

      if(scheduler_state == AWAKE){
           scheduler();
       }
       __WFI();


    }
}
