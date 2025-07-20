/*
 * ===============================================================================
 * GREENHOUSE AUTOMATION SYSTEM - MAIN CONTROLLER
 * ===============================================================================
 * 
 * This is the main control program for an automated greenhouse monitoring system.
 * The system manages:
 * - Light control (grow lights with brightness control)
 * - Temperature monitoring and control
 * - Air quality monitoring
 * - Water management system
 * - User interface through LCD display and option menu
 * - IoT communication for remote monitoring
 * 
 * Hardware Platform: Texas Instruments MSP432P401R microcontroller
 * Display: 128x128 LCD with Crystalfontz ST7735 controller
 * Sensors: Light sensor (OPT3001), temperature, air quality, water level
 * 
 * The system runs on a task scheduler that periodically checks sensors
 * and adjusts environmental controls automatically.
 */

// MICROCONTROLLER AND HARDWARE INCLUDES
#include "msp.h"                                    // MSP432 basic definitions and registers
#include <ti/devices/msp432p4xx/driverlib/driverlib.h>  // TI driver library for hardware control
#include <ti/grlib/grlib.h>                         // Graphics library for LCD display

// SYSTEM MODULE INCLUDES - Each module controls a specific aspect of the greenhouse
#include "environment_systems/temperature.h"
#include "light_system/growing_light.h"
#include "environment_systems/air_quality.h"
#include "environment_systems/buzzer.h"
#include "water_management/water_init.h"
#include "water_management/water_reading.h"
#include "water_management/pump_management.h"

// SYSTEM INFRASTRUCTURE INCLUDES
#include "scheduling/scheduler.h"
#include "scheduling/timer.h"
#include "option_menu/option_menu.h"
#include "option_menu/options.h"
#include "uart_communication/uart_comm.h"
#include "adc/adc.h"

// HARDWARE ABSTRACTION LAYER INCLUDES
#include "../lib/HAL_I2C.h"                        // I2C communication library
#include "../include/LcdDriver/Crystalfontz128x128_ST7735.h"  // LCD driver

// STANDARD C LIBRARY
#include <stdio.h>                                 // Standard input/output functions

// Graphics context for LCD display operations - stores display settings and state
Graphics_Context g_sContext;


/************************************
 *  DISPLAY INITIALIZATION FUNCTION *
 ************************************/
/**
 * @brief Initializes the LCD graphics display system
 * 
 * This function sets up the 128x128 pixel LCD display that shows the user interface.
 * It configures the display orientation, colors, font, and clears the screen.
 * The display is used to show sensor readings, system status, and menu options.
 */
void _graphicsInit() {
    // STEP 1: Initialize the physical LCD hardware
    Crystalfontz128x128_Init();

    // STEP 2: Set the display orientation (which way is "up")
    Crystalfontz128x128_SetOrientation(LCD_ORIENTATION_UP);

    // STEP 3: Initialize the graphics context (like setting up a canvas for drawing)
    Graphics_initContext(&g_sContext, &g_sCrystalfontz128x128, &g_sCrystalfontz128x128_funcs);
    
    // STEP 4: Configure display appearance
    Graphics_setForegroundColor(&g_sContext, GRAPHICS_COLOR_RED);    // Text color
    Graphics_setBackgroundColor(&g_sContext, GRAPHICS_COLOR_WHITE);  // Background color
    GrContextFontSet(&g_sContext, &g_sFontFixed6x8);                // Font size and style
    
    // STEP 5: Clear the display to start with a blank screen
    Graphics_clearDisplay(&g_sContext);
}

/*****************************************
 *   USER INTERFACE MENU CONFIGURATION   *
 *****************************************/

/**
 * @brief Creates and configures all user interface menu options
 * 
 * This function builds the complete menu system that allows users to:
 * - Control and configure the grow light system
 * - Adjust temperature monitoring settings
 * - Configure air quality monitoring
 * - Control the alert buzzer
 * - Manage water system settings
 * 
 * Each menu option is linked to a specific control function that gets called
 * when the user changes that setting.
 */
void add_tasks_to_option_menu() {
    int err;  // Error flag for option creation validation

    /*
     * LIGHT SYSTEM CONFIGURATION SECTION
     */

    /*
     * GROW LIGHT POWER CONTROL
     * Allows manual on/off control of the grow lights (only works in manual mode)
     */
    OptionUnion opt_led_s_power = option_u_new_switch(false);  // Create switch, initially off
    Option leds_switch = option_new("power leds",              // Display name in menu
                                   SWITCH,                      // Type of control (on/off switch)
                                   opt_led_s_power,            // The switch value
                                   power_on_or_off,            // Function to call when changed
                                   to_string_switch_default);  // How to display the value
    option_menu_push_option(leds_switch);  // Add to the menu

    /*
     * LIGHT CONTROL MODE SELECTION
     * Switches between automatic (sensor-controlled) and manual (user-controlled) modes
     */
    OptionUnion opt_led_s_mode = option_u_new_switch(false);   // Create switch for mode
    Option leds_manual = option_new("light mode",              // Display name
                                   SWITCH,                      // Switch type
                                   opt_led_s_mode,             // Switch value
                                   grow_light_set_mode,        // Function to change mode
                                   to_string_manual_auto);     // Display "Manual/Auto"
    option_menu_push_option(leds_manual);

    /*
     * LIGHT SENSOR UPDATE TIMER
     * Controls how often the system checks the light sensor and updates grow lights
     */
    OptionUnion opt_led_t = option_u_new_timer("10s", &err);   // Default: check every 10 seconds
    if (err == 1) {  // Check if the timer value was invalid
        #ifdef DEBUG
        puts("10s is not on the list of possible timing values\n");
        #endif
        return;  // Exit if we can't create this option
    }
    Option leds_timer = option_new("light timer",              // Display name
                                  TIMER,                        // Timer type control
                                  opt_led_t,                   // Timer value
                                  update_light_timer,          // Function to update timer
                                  to_string_timer_default);    // How to display timer value
    option_menu_push_option(leds_timer);

    /*
     * LIGHT BRIGHTNESS THRESHOLD
     * Sets the natural light level at which grow lights turn on/off
     * Higher values -> grow lights turn on in darker conditions
     * Lower values -> grow lights only turn on when it's very dark
     */
    OptionUnion opt_led_thr = option_u_new_threshold(grow_light_get_threshold(), // Current threshold
                                                    MIN_BRIGHTNESS,              // Minimum allowed value
                                                    MAX_BRIGHTNESS,              // Maximum allowed value
                                                    20,                          // Step size for adjustment
                                                    &err);                       // Error flag
    if (err == 1) {  // Check if current threshold is out of range
        #ifdef DEBUG
        puts("Current threshold exceeds the range\n");
        #endif
        return;
    }
    Option leds_threshold = option_new("change light threshold", // Display name
                                    THRESHOLD,                   // Threshold adjustment control
                                    opt_led_thr,                 // Threshold settings
                                    grow_light_set_threshold,    // Function to update threshold
                                    to_string_threshold_default);// How to display threshold
    option_menu_push_option(leds_threshold);

    /*
     * TEMPERATURE SYSTEM CONFIGURATION SECTION
     */

    /*
     * TEMPERATURE SENSOR UPDATE TIMER
     * Controls how often the system checks temperature sensors
     */
    OptionUnion opt_temp_t = option_u_new_timer("5s", &err);  // Check every 5 seconds
    if (err == 1) {
        #ifdef DEBUG
        puts("5s is not on the list of possible timing values\n");
        #endif
        return;
    }
    Option temp_timer = option_new("temp sensor timer",         // Display name
                                  TIMER,                        // Timer control
                                  opt_temp_t,                  // Timer value
                                  update_temperature_timer,    // Function to update timer
                                  to_string_timer_default);    // Display format
    option_menu_push_option(temp_timer);

    /*
     * HIGH TEMPERATURE THRESHOLD
     * Temperature level that triggers cooling or alerts (e.g., turn on fans, open vents)
     */
    OptionUnion opt_temp_higher_thr = option_u_new_threshold(temp_get_higher_threshold(), // Current value
                                                            0,                           // Minimum (0°C)
                                                            42,                          // Maximum (42°C)
                                                            5,                           // Step size (5°C)
                                                            &err);
    if (err == 1) {
        #ifdef DEBUG
        puts("Current threshold exceeds the range\n");
        #endif
        return;
    }
    Option temp_higher_threshold = option_new("change temp higher threshold",
                                             THRESHOLD,
                                             opt_temp_higher_thr,
                                             temp_set_higher_threshold,
                                             to_string_threshold_default);
    option_menu_push_option(temp_higher_threshold);

    /*
     * LOW TEMPERATURE THRESHOLD  
     * Temperature level that triggers alerts indicating it's too cold
     */
    OptionUnion opt_temp_lower_thr = option_u_new_threshold(temp_get_lower_threshold(),
                                                           0,      // Minimum (0°C)
                                                           40,     // Maximum (42°C)  
                                                           5,      // Step size (5°C)
                                                           &err);
    if (err == 1) {
        #ifdef DEBUG
        puts("Current threshold exceeds the range\n");
        #endif
        return;
    }
    Option temp_lower_threshold = option_new("change temp lower threshold",
                                            THRESHOLD,
                                            opt_temp_lower_thr,
                                            temp_set_lower_threshold,
                                            to_string_threshold_default);
    option_menu_push_option(temp_lower_threshold);

    /*
     * AIR QUALITY SENSOR CONFIGURATION SECTION
     */
    
    /*
     * AIR QUALITY SENSOR UPDATE TIMER
     * Controls how often the system checks air quality sensors
     * Air quality changes more slowly than temperature, so we check less frequently
     */
    OptionUnion opt_air_t = option_u_new_timer("20s", &err);  // Check every 20 seconds
    if (err == 1) {
        #ifdef DEBUG
        puts("20s is not on the list of possible timing values\n");
        #endif
        return;
    }
    Option air_timer = option_new("air system timer",      // Display name
                                 TIMER,                    // Timer control
                                 opt_air_t,                // Timer value  
                                 update_air_timer,         // Function to update timer
                                 to_string_timer_default); // Display format
    option_menu_push_option(air_timer);

    /*
     * AIR QUALITY THRESHOLD
     * Air quality level that triggers ventilation or alerts
     * Higher values = more sensitive (triggers at better air quality)
     * Lower values = less sensitive (only triggers when air is poor)
     */
    OptionUnion opt_air_thr = option_u_new_threshold(air_get_threshold(), // Current threshold
                                                    0,                    // Minimum value
                                                    50,                   // Maximum value  
                                                    5,                    // Step size
                                                    &err);
    if (err == 1) {
        #ifdef DEBUG
        puts("Current threshold exceeds the range\n");
        #endif
        return;
    }
    Option air_threshold = option_new("change air threshold",      // Display name
                                     THRESHOLD,                    // Threshold control
                                     opt_air_thr,                 // Threshold settings
                                     air_set_threshold,           // Function to update threshold
                                     to_string_threshold_default); // Display format
    option_menu_push_option(air_threshold);

    /*
     * BUZZER/ALERT SYSTEM CONFIGURATION SECTION
     */

    /*
     * BUZZER POWER CONTROL
     * Manual on/off control for the alert buzzer
     */
    OptionUnion opt_buzzer_s_power = option_u_new_switch(false);   // Initially off
    Option buzzer_switch = option_new("power buzzer",             // Display name
                                     SWITCH,                       // Switch control
                                     opt_buzzer_s_power,          // Switch value
                                     manual_power_buzzer,         // Function to control buzzer
                                     to_string_switch_default);   // Display format
    option_menu_push_option(buzzer_switch);  // Fixed: was adding leds_switch instead

    /*
     * BUZZER MODE CONTROL
     * Switches between automatic (triggered by system alerts) and manual modes
     */
    OptionUnion opt_buzzer_s_mode = option_u_new_switch(false);
    Option buzzer_manual = option_new("buzzer mode",              // Display name
                                     SWITCH,                      // Switch control
                                     opt_buzzer_s_mode,          // Switch value
                                     set_buzzer_mode,            // Function to change mode
                                     to_string_manual_auto);     // Display "Manual/Auto"
    option_menu_push_option(buzzer_manual);  // Fixed: was adding leds_manual instead

    /*
     * WATER SYSTEM CONFIGURATION SECTION
     */
    add_water_options();  // This function is defined in the water management module
}

/**************************************
 *   SYSTEM INITIALIZATION FUNCTION   *
 **************************************/

/**
 * @brief Initializes all hardware components and system modules
 * 
 * This is the main initialization function that sets up the entire greenhouse system.
 * It must be called once at startup before the main control loop begins.
 * 
 * INITIALIZATION ORDER IS CRITICAL:
 * 1. Basic microcontroller setup (clocks, interrupts)
 * 2. Core system services (scheduler, timers, ADC)
 * 3. User interface components (display, menu)
 * 4. Communication systems (I2C, UART, IoT)
 * 5. Environmental control modules (lights, temperature, air, water)
 * 6. Interrupt enabling (allows system to respond to events)
 */
void _hwInit() {
    // STEP 1: MICROCONTROLLER BASIC SETUP
    
    // Disable the watchdog timer (safety feature that resets system if it hangs)
    // We disable it because our main loop runs continuously and handles its own timing
    WDT_A_holdTimer();
    
    // Temporarily disable all interrupts during initialization
    // This prevents interrupts from firing before everything is properly set up
    Interrupt_disableMaster();

    // STEP 2: CLOCK SYSTEM CONFIGURATION
    
    // Set the main CPU clock (DCO - Digitally Controlled Oscillator) to 3 MHz
    // This determines how fast the microcontroller processes instructions
    CS_setDCOCenteredFrequency(CS_DCO_FREQUENCY_3);
    
    // Configure SMCLK (Sub-Main Clock) to also run at 3 MHz
    // SMCLK is used by peripheral modules like timers and communication interfaces
    CS_initClockSignal(CS_SMCLK,           // Which clock signal to configure
                      CS_DCOCLK_SELECT,    // Use DCO as the source
                      CS_CLOCK_DIVIDER_1); // Don't divide the frequency (keep it at 3 MHz)

    // STEP 3: CORE SYSTEM SERVICES INITIALIZATION
    
    // Initialize the task scheduler - this manages automatic background tasks
    // like sensor reading, light control, temperature monitoring, etc.
    scheduler_init();
    
    // Initialize system timing functions used by the scheduler
    timer_init();
    
    // Initialize ADC (Analog-to-Digital Converter) for reading analog sensors
    // This allows us to read continuous values like temperature, light levels, etc.
    adc_init();

    // STEP 4: USER INTERFACE INITIALIZATION
    
    // Initialize the option menu system
    // _graphicsInit();

    // Set up the menu system that allows users to configure system settings
    option_menu_init(&g_sContext);  // Pass the graphics context for display
    
    // Create all the menu options for controlling different system aspects
    add_tasks_to_option_menu();

    // STEP 5: COMMUNICATION SYSTEMS INITIALIZATION
    
    // Initialize I2C communication bus (used for sensors like light sensor)
    I2C_init();
    Init_I2C_GPIO();  // Configure the GPIO pins used for I2C communication
    
    // Initialize IoT communication GPIO pins for sending data to external systems
    init_GPIOs_IOT();
    
    // UART communication initialization
    // This sets up serial communication with the computer running the python script
    uart_init();

    // STEP 6: ENVIRONMENTAL CONTROL MODULES INITIALIZATION
    
    // Initialize the buzzer for audio alerts and notifications
    init_buzzer();
    
    // Initialize the grow light control system (automatic light management)
    grow_light_init();
    
    // Initialize temperature sensor and control system
    temp_sensor_init();
    
    // Initialize air quality monitoring system
    air_init();
    
    // Initialize water management system (pumps, sensors, irrigation control)
    water_init();

    // STEP 7: INTERRUPT SYSTEM ACTIVATION
    
    // Enable specific interrupt for Timer_A1 (used by the grow light PWM system)
    Interrupt_enableInterrupt(INT_TA1_N);
    
    // Re-enable global interrupts - system is now ready to respond to events
    // This MUST be the last step - everything must be initialized before enabling interrupts
    Interrupt_enableMaster();
}
/*
* MAIN PROGRAM ENTRY POINT
*/

/**
 * @brief Main program entry point and control loop
 * 
 * This is where the program starts executing when the microcontroller powers on.
 * It performs system initialization and then runs the main control loop that
 * keeps the greenhouse system operating continuously.
 * 
 * PROGRAM FLOW:
 * 1. Disable watchdog timer (safety feature)
 * 2. Initialize all hardware and software systems  
 * 3. Enter infinite loop that runs the task scheduler
 * 4. The scheduler automatically handles all periodic tasks like:
 *    - Reading sensors
 *    - Adjusting lights based on light levels
 *    - Monitoring temperature and air quality
 *    - Controlling water pumps
 *    - Updating the user interface
 * 
 * The main loop never exits - the system runs continuously until power is removed.
 */
void main(void) {
    // STEP 1: DISABLE WATCHDOG TIMER
    
    // The watchdog timer is a safety feature that automatically resets the
    // microcontroller if the program gets stuck or stops responding.
    // We disable it here because our program runs an infinite loop by design.
    //
    // WDT_A_CTL register controls the watchdog timer:
    // - WDT_A_CTL_PW: Password required to modify watchdog settings
    // - WDT_A_CTL_HOLD: Stop the watchdog timer from counting
    WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD;

    // STEP 2: SYSTEM INITIALIZATION
    
    // Initialize all hardware components, sensors, communication systems,
    // and software modules. This sets up everything the greenhouse needs to operate.
    _hwInit();

    // STEP 3: MAIN CONTROL LOOP
    
    // Enter the infinite main loop - this keeps the system running forever
    while (1) {
        // Check if the scheduler is awake and ready to process tasks
        // The scheduler can be in AWAKE or SLEEP states to save power
        if (scheduler_state == AWAKE) {
            // Runs the task scheduler
            scheduler();
        }
        
        // Note: When scheduler_state != AWAKE, the system enters a low-power mode
        // This saves battery power when no immediate tasks need to be performed
        // The system will wake up automatically when:
        // - A timer expires (time for the next sensor reading)
        // - A user presses a button
        // - An interrupt occurs
    }
    return;
}
