/*
 * adc.h
 *
 *  Created on: Jul 17, 2025
 *      Author: riginel
 */

#ifndef INCLUDE_ADC_ADC_H_
#define INCLUDE_ADC_ADC_H_

/*
 * JOYSTICK Y-AXIS CONFIGURATION (Vertical movement)
 * Memory location 0, connected to ADC input A9 (P4.4)
 */
#define Y_AXIS_PORT  GPIO_PORT_P4
#define Y_AXIS_PIN  GPIO_PIN4
#define Y_AXIS_MEM ADC_MEM0
#define Y_AXIS_INPUT ADC_INPUT_A9

/*
 * JOYSTICK X-AXIS CONFIGURATION (Horizontal movement)
 * Memory location 1, connected to ADC input A15 (P6.0)
 */
#define X_AXIS_PORT GPIO_PORT_P6
#define X_AXIS_PIN GPIO_PIN0
#define X_AXIS_MEM ADC_MEM1
#define X_AXIS_INPUT ADC_INPUT_A15

/*
 * AIR QUALITY SENSOR CONFIGURATION (MQ135 gas sensor)
 * Memory location 2, connected to ADC input A10 (P4.3)
 */
#define AIR_SENSOR_PORT GPIO_PORT_P4
#define AIR_SENSOR_PIN  GPIO_PIN3
#define AIR_SENSOR_MEM  ADC_MEM2
#define AIR_SENSOR_INPUT ADC_INPUT_A10


/*
 * MAIN WATER TANK SENSOR CONFIGURATION
 * Memory location 3, connected to ADC input A0 (P5.4)
 */
#define TANK_SENSOR_PORT GPIO_PORT_P5
#define TANK_SENSOR_PIN GPIO_PIN4
#define TANK_SENSOR_MEM ADC_MEM3
#define TANK_SENSOR_INPUT ADC_INPUT_A0

/*
 * BACKUP RESERVOIR SENSOR CONFIGURATION
 * Memory location 4, connected to ADC input A1 (P5.5)
 */
#define RESERVOIRE_SENSOR_PORT GPIO_PORT_P5
#define RESERVOIRE_SENSOR_PIN GPIO_PIN5
#define RESERVOIRE_SENSOR_MEM ADC_MEM4
#define RESERVOIRE_SENSOR_INPUT ADC_INPUT_A1

/**
 * @brief Initializes the ADC system for multi-channel analog sensor reading
 * 
 * This function sets up the ADC to automatically read from 5 different analog
 * sensors in a continuous cycle. It configures GPIO pins, ADC settings, memory
 * locations, and interrupt handling.
 * 
 * INITIALIZATION SEQUENCE:
 * 1. Configure GPIO pins for analog input
 * 2. Enable and configure the ADC module
 * 3. Set up multi-sequence scanning mode
 * 4. Configure memory locations for each sensor
 * 5. Enable interrupts for automatic processing
 * 6. Start continuous conversion process
 */
void adc_init();

/**
 * @brief ADC interrupt service routine - processes completed conversions
 * 
 * This function is automatically called by the microcontroller hardware
 * whenever the ADC completes a full conversion sequence (all 5 sensors).
 * It's a critical function that must execute quickly to avoid interfering
 * with other system operations.
 * 
 * INTERRUPT OPERATION:
 * 1. Hardware automatically calls this function when ADC_INT4 triggers
 * 2. Function reads interrupt status to determine what caused the interrupt
 * 3. Clears interrupt flags to prepare for next conversion cycle
 * 4. Dispatches sensor data to appropriate processing functions
 * 5. Returns control to the main program
 */
void ADC14_IRQHandler(void);

#endif /* INCLUDE_ADC_ADC_H_ */
