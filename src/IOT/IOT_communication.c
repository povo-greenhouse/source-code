/*
 * IOT_communication.c
 *
 *  Created on: 25 giu 2025
 *      Author: vince
 *
 * IOT COMMUNICATION MODULE - GPIO-BASED DATA TRANSMISSION SYSTEM
 * 
 * OVERVIEW:
 * This module implements a custom GPIO-based communication protocol for transmitting greenhouse sensor 
 * data to external IoT monitoring systems. Instead of using traditional serial protocols (UART, SPI, I2C), this
 * system uses parallel GPIO pins to encode sensor information.
 *
 * COMMUNICATION PROTOCOL DESIGN:
 * The system uses 7 GPIO pins to transmit structured data packets:
 * 
 * PIN ALLOCATION:
 * - 3 Data Pins (P2.3, P6.7, P3.5): Encode sensor type (8 possible types)
 * - 1 Status Pin (P3.7): Indicates sensor active/inactive state
 * - 2 Value Pins (P5.7, P3.0): Encode sensor readings (4 possible values)
 * - 1 Select Pin (P3.6): Control signal for data transmission timing
 *
 * DATA ENCODING PROTOCOL:
 * Each transmission includes:
 * 1. SENSOR TYPE (3-bit code): Identifies which sensor is reporting
 * 2. STATUS FLAG (1-bit): Active/inactive state of the sensor
 * 3. VALUE CODE (2-bit): Categorical reading from the sensor
 * 4. TIMING CONTROL: Select pin manages transmission synchronization
 *
 * SUPPORTED GREENHOUSE SYSTEMS:
 * - Air Quality Monitoring (code 1)
 * - Temperature Control (code 2) 
 * - Water Tank Level (code 3)
 * - Reservoir Management (code 4)
 * - Growing Light Control (code 5)
 * - Water Pump 1 Control (code 6)
 * - Water Pump 2 Control (code 7)
 */

#include "IOT/IOT_communication.h"
#include "msp.h"

/*
 * GPIO INITIALIZATION FOR IOT COMMUNICATION PROTOCOL 
 */

void init_GPIOs_IOT(){
    
    // DATA BIT 2 (MSB) - P2.3 CONFIGURATION
    // This pin represents the Most Significant Bit (MSB) of the 3-bit sensor code
    // Used to distinguish between sensor types
    
    // Configure P2.3 as standard GPIO
    P2->SEL0 &= ~BIT3;
    P2->SEL1 &= ~BIT3;   
    // Set as output to drive signals to receiver
    P2->DIR  |= BIT3;
    // Initialize to HIGH: Default logic 1 state    
    P2->OUT |= BIT3;

    // DATA BIT 1 (MIDDLE) - P6.7 CONFIGURATION  
    // This pin represents the middle bit of the 3-bit sensor code

    // Configure P6.7 as standard GPIO
    P6->SEL0 &= ~BIT7;
    P6->SEL1 &= ~BIT7;
    // Set as output to enable signal transmission
    P6->DIR  |= BIT7;
    // Initialize to HIGH: Known starting state
    P6->OUT |= BIT7;

    // DATA BIT 0 (LSB) - P3.5 CONFIGURATION
    // This pin represents the Least Significant Bit (LSB) of the sensor code
    // Provides finest granularity for sensor type encoding

    // Configure P3.5 as standard GPIO
    P3->SEL0 &= ~BIT5;
    P3->SEL1 &= ~BIT5;
    // Set as output to drive external receiver
    P3->DIR  |= BIT5;
    // Initialize to HIGH: Known starting state
    P3->OUT|=BIT5;

    // STATUS/ACTIVITY FLAG - P3.7 CONFIGURATION
    // This pin indicates whether the reporting sensor is currently active
    // HIGH = sensor inactive/disabled, LOW = sensor active/operational
    
    // Configure P3.7 as standard GPIO
    P3->SEL0 &= ~BIT7;
    P3->SEL1 &= ~BIT7;
    // Set as output to transmit status information
    P3->DIR  |= BIT7;
    // Initialize to HIGH: Default "inactive" state
    P3->OUT |= BIT7;

    // VALUE BIT 1 (MSB) - P5.7 CONFIGURATION
    // This pin represents the MSB of the 2-bit value encoding
    // Allows encoding of 4 different sensor reading categories (00, 01, 10, 11)
    
    // Configure P5.7 as standard GPIO
    P5->SEL0 &= ~BIT7;
    P5->SEL1 &= ~BIT7;
    // Set as output to drive value information
    P5->DIR  |= BIT7;
    // Initialize to HIGH: Default value state
    P5->OUT|=BIT7;

    // VALUE BIT 0 (LSB) - P3.0 CONFIGURATION
    // This pin represents the LSB of the 2-bit value encoding
    // Combined with P5.7, provides 4 distinct value categories per sensor
    
    // Configure P3.0 as standard GPIO
    P3->SEL0 &= ~BIT0;
    P3->SEL1 &= ~BIT0;
    // Set as output to transmit value data
    P3->DIR  |= BIT0;
    // Initialize to HIGH: Known default value
    P3->OUT|=BIT0;

    // TRANSMISSION CONTROL/SELECT - P3.6 CONFIGURATION
    // This pin controls the timing of data transmission
    // Acts as a "data valid" or "chip select" signal for the receiving system
    // HIGH = data stable/valid, LOW = transmission in progress
    
    // Configure P3.6 as standard GPIO
    P3->SEL0 &= ~BIT6;
    P3->SEL1 &= ~BIT6;
    // Set as output to control transmission timing
    P3->DIR  |= BIT6;
    // Initialize to HIGH: "Ready" state for receiver
    P3->OUT|=BIT6;
}

void send_data(int code, bool active, int value){
    
    // STEP 1: SIGNAL TRANSMISSION START
    // Assert the select/control signal to indicate data transmission beginning
    // This allows the receiving system to prepare for incoming data
    P3->OUT |= BIT6;  // Set select HIGH: "Data incoming, prepare to receive"
    
    // ===============================================================================
    // STEP 2: ENCODE SENSOR TYPE (3-BIT PARALLEL CODE)
    // ===============================================================================
    // Use 3 GPIO pins to encode which sensor is transmitting data
    // This creates a unique binary signature for each sensor type
    
    switch(code){
        // AIR QUALITY SENSOR - Code 1 (Binary: 110)
        // ===================================================================
        case 1: // P2.3=1, P6.7=1, P3.5=0 → Air quality monitoring system
                    P2->OUT |=  BIT3;    // Data bit 2 = HIGH
                    P6->OUT |=  BIT7;    // Data bit 1 = HIGH  
                    P3->OUT &= ~BIT5;    // Data bit 0 = LOW
                    break;
                    
        // TEMPERATURE SENSOR - Code 2 (Binary: 101)
        // ===================================================================            
        case 2: // P2.3=1, P6.7=0, P3.5=1 → Temperature monitoring system
                    P2->OUT |=  BIT3;    // Data bit 2 = HIGH
                    P6->OUT &= ~BIT7;    // Data bit 1 = LOW
                    P3->OUT |=  BIT5;    // Data bit 0 = HIGH
                    break;
                    
        // WATER TANK LEVEL - Code 3 (Binary: 100)
        // ===================================================================
        case 3: // P2.3=1, P6.7=0, P3.5=0 → Water tank level monitoring
                    P2->OUT |=  BIT3;    // Data bit 2 = HIGH
                    P6->OUT &= ~BIT7;    // Data bit 1 = LOW
                    P3->OUT &= ~BIT5;    // Data bit 0 = LOW
                    break;
                    
        // RESERVOIR MANAGEMENT - Code 4 (Binary: 011)
        // ===================================================================
        case 4: // P2.3=0, P6.7=1, P3.5=1 → Water reservoir system
                    P2->OUT &= ~BIT3;    // Data bit 2 = LOW
                    P6->OUT |=  BIT7;    // Data bit 1 = HIGH
                    P3->OUT |=  BIT5;    // Data bit 0 = HIGH
                    break;
                    
        // GROWING LIGHT SYSTEM - Code 5 (Binary: 010)
        // ===================================================================
        case 5: // P2.3=0, P6.7=1, P3.5=0 → LED grow light control
                    P2->OUT &= ~BIT3;    // Data bit 2 = LOW
                    P6->OUT |=  BIT7;    // Data bit 1 = HIGH
                    P3->OUT &= ~BIT5;    // Data bit 0 = LOW
                    break;
                    
        // WATER PUMP 1 - Code 6 (Binary: 001)
        // ===================================================================
        case 6: // P2.3=0, P6.7=0, P3.5=1 → Primary water pump control
                    P2->OUT &= ~BIT3;    // Data bit 2 = LOW
                    P6->OUT &= ~BIT7;    // Data bit 1 = LOW
                    P3->OUT |=  BIT5;    // Data bit 0 = HIGH
                    break;
                    
        // WATER PUMP 2 - Code 7 (Binary: 000)
        // ===================================================================
        case 7: // P2.3=0, P6.7=0, P3.5=0 → Secondary water pump control
                    P2->OUT &= ~BIT3;    // Data bit 2 = LOW
                    P6->OUT &= ~BIT7;    // Data bit 1 = LOW
                    P3->OUT &= ~BIT5;    // Data bit 0 = LOW
                    break;
            }
            
    // ===============================================================================
    // STEP 3: ENCODE SENSOR STATUS (ACTIVE/INACTIVE FLAG)
    // ===============================================================================
    // Single bit indicating whether the reporting sensor is operational
    // This allows the monitoring system to distinguish between:
    // - Actual sensor readings (active=true)
    // - System status updates (active=false)
    
    if (active)
        P3->OUT &= ~BIT7;  // ACTIVE: Set status pin LOW (sensor operational)
    else
        P3->OUT |= BIT7;   // INACTIVE: Set status pin HIGH (sensor disabled)

    // ===============================================================================
    // STEP 4: ENCODE MEASUREMENT VALUE (2-BIT CATEGORICAL CODE)
    // ===============================================================================
    // Use 2 pins to encode sensor reading into 4 possible categories
    // This provides sufficient granularity for most greenhouse monitoring needs
    
    switch(value){
        // VALUE CATEGORY 1 - Code 1 (Binary: 11)
        // ===================================================================
        case 1: // P5.7=1, P3.0=1 → High/Critical/Above threshold reading
                    P5->OUT |= BIT7;     // Value bit 1 = HIGH
                    P3->OUT |= BIT0;     // Value bit 0 = HIGH
                    break;

        // VALUE CATEGORY 2 - Code 2 (Binary: 10)  
        // ===================================================================
        case 2: // P5.7=1, P3.0=0 → Normal/Optimal/Within range reading
                    P5->OUT |= BIT7;     // Value bit 1 = HIGH
                    P3->OUT &= ~BIT0;    // Value bit 0 = LOW
                    break;

        // VALUE CATEGORY 3 - Code 3 (Binary: 01)
        // ===================================================================
        case 3: // P5.7=0, P3.0=1 → Low/Warning/Below threshold reading
                    P5->OUT &= ~BIT7;    // Value bit 1 = LOW
                    P3->OUT |= BIT0;     // Value bit 0 = HIGH
                    break;

        // VALUE CATEGORY 4 - Code 4 (Binary: 00)
        // ===================================================================
        case 4: // P5.7=0, P3.0=0 → Error/Unknown/Invalid reading
                    P5->OUT &= ~BIT7;    // Value bit 1 = LOW
                    P3->OUT &= ~BIT0;    // Value bit 0 = LOW
                    break;

        default: 
            // SAFETY: If invalid value provided, default to error state
            P5->OUT &= ~BIT7;    // Value bit 1 = LOW
            P3->OUT &= ~BIT0;    // Value bit 0 = LOW
            break;
    }
    
    // ===============================================================================
    // STEP 5: SIGNAL TRANSMISSION COMPLETE
    // ===============================================================================
    // Deassert the select signal to indicate end of data transmission
    // This tells the receiver that all data bits are now stable and valid
    P3->OUT &= ~BIT6;  // Set select LOW: "Transmission complete, data valid"

    // ===============================================================================
    // STEP 6: RESET GPIO PINS TO DEFAULT STATE
    // ===============================================================================
    // Return all pins to their default HIGH state for the next transmission
    // This ensures a clean starting point and prevents data corruption
    init_GPIOs_IOT();  // Reset all pins to default configuration
}
