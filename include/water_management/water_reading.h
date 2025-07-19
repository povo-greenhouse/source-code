/*
 * water_reading.h
 *
 *  Created on: 24 giu 2025
 *      Author: vince
 */
#include <stdbool.h>
#include <stdint.h>
#ifndef INCLUDE_WATER_MANAGEMENT_WATER_READING_H_
#define INCLUDE_WATER_MANAGEMENT_WATER_READING_H_

#define TANK_SENSOR_PORT GPIO_PORT_P5
#define TANK_SENSOR_PIN GPIO_PIN4
#define TANK_SENSOR_MEM ADC_MEM3
#define TANK_SENSOR_INPUT ADC_INPUT_A0

#define RESERVOIRE_SENSOR_PORT GPIO_PORT_P5
#define RESERVOIRE_SENSOR_PIN GPIO_PIN5
#define RESERVOIRE_SENSOR_MEM ADC_MEM4
#define RESERVOIRE_SENSOR_INPUT ADC_INPUT_A1
//#define N_SAMPLES 4
int index_tank, index_reservoire;
bool block;

//void adc_init(void);
#define RESERVOIRE_TRESH_DEFAULT 25


#define RESERVOIRE_EMPTY_THRESH_DEFAULT 1000
#define RESERVOIRE_LOW_THRESH_DEFAULT 3000
#define RESERVOIRE_MODERATE_THRESH_DEFAULT 4500
#define TANK_EMPTY_THRESH_DEFAULT 6000

#define READ_TANK_TIME_DEFAULT 2000
#define READ_RESERVOIRE_TIME_DEFAULT 1500



void read_tank(void);
void read_reservoire(void);

void add_water_reading_options();

void upd_res_empty_threshold(int32_t);
void upd_res_low_threshold(int32_t);
void upd_res_moderate_threshold(int32_t);
void upd_tank_empty_threshold(int32_t);
void upd_tank_read_time(int32_t);
void upd_res_read_time(int32_t);

void handle_water_level_interrupt(uint64_t);


#endif /* INCLUDE_WATER_MANAGEMENT_WATER_READING_H_ */
