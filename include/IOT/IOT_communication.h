/*
 * IOT_communication.h
 *
 *  Created on: 25 giu 2025
 *      Author: vince
 */
#include <stdbool.h>
#ifndef INCLUDE_IOT_IOT_COMMUNICATION_H_
#define INCLUDE_IOT_IOT_COMMUNICATION_H_

/**
 * @brief Configures GPIO pins for custom IoT communication protocol
 * 
 * This function initializes 7 GPIO pins on the MSP432P401R microcontroller
 * to implement a parallel data transmission system for greenhouse monitoring.
 * Each pin is carefully configured to ensure reliable digital communication
 * with external IoT receiving hardware.
 * 
 * GPIO CONFIGURATION SEQUENCE:
 * For each pin, we perform a 4-step configuration process:
 * 1. FUNCTION SELECTION: Configure pin as standard GPIO (not special function)
 * 2. DIRECTION SETTING: Configure pin as output for data transmission
 * 3. INITIAL STATE: Set pin to HIGH (logic 1) as default state
 * 4. ELECTRICAL CHARACTERISTICS: Ensure proper drive strength and pull-ups
 * 
 * PIN MAPPING AND PHYSICAL CONNECTIONS:
 * 
 * DATA ENCODING PINS (3-bit sensor type encoding):
 * - P2.3 (Bit 2): Most Significant Bit of sensor type code
 * - P6.7 (Bit 1): Middle bit of sensor type code  
 * - P3.5 (Bit 0): Least Significant Bit of sensor type code
 * 
 * STATUS AND CONTROL PINS:
 * - P3.7: Sensor active/inactive status flag
 * - P5.7 (Bit 1): Most Significant Bit of value encoding
 * - P3.0 (Bit 0): Least Significant Bit of value encoding
 * - P3.6: Transmission control/select signal
 */
void init_GPIOs_IOT();

/**
 * @brief Transmits greenhouse sensor data using custom GPIO protocol
 * 
 * This function implements the core data transmission logic for sending
 * structured sensor information to external IoT monitoring systems.
 * The protocol uses parallel GPIO pins to encode sensor type, status,
 * and measurement values in a single atomic transmission.
 * 
 * TRANSMISSION PROTOCOL SEQUENCE:
 * 1. ENCODE SENSOR TYPE: Set 3-bit code identifying sensor source
 * 2. ENCODE STATUS: Set activity flag for sensor state
 * 3. ENCODE VALUE: Set 2-bit measurement category code
 * 4. ASSERT SELECT: Signal start of transmission
 * 5. RESET PINS: Return all pins to default state
 * 
 * PARAMETER ENCODING:
 * 
 * @param code Sensor type identifier (1-7):
 *   1 = Air Quality Monitor    (binary: 110)
 *   2 = Temperature Sensor     (binary: 101) 
 *   3 = Water Tank Level       (binary: 100)
 *   4 = Reservoir Management   (binary: 011)
 *   5 = Growing Light System   (binary: 010)
 *   6 = Water Pump 1          (binary: 001)
 *   7 = Water Pump 2          (binary: 000)
 * 
 * @param active Sensor operational status:
 *   true  = Sensor is active/operational -> P3.7 = LOW
 *   false = Sensor is inactive/disabled -> P3.7 = HIGH
 * 
 * @param value Measurement category (1-4):
 *   1 = Category 1 (binary: 11) - High/Critical/Above threshold
 *   2 = Category 2 (binary: 10) - Normal/Optimal/Within range  
 *   3 = Category 3 (binary: 01) - Low/Warning/Below threshold
 *   4 = Category 4 (binary: 00) - Unknown/No reading
 * 
 * INTEGRATION WITH GREENHOUSE SYSTEMS:
 * This function is called by various sensor modules:
 * - Temperature monitoring
 * - Light level adjustments
 * - Water level alerts
 * - Air quality warnings
 */
void send_data(int,bool,int);

#endif /* INCLUDE_IOT_IOT_COMMUNICATION_H_ */
