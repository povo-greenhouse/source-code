/*
 * water_reading.h
 *
 *  Created on: 24 giu 2025
 *      Author: vince
 */
#include <stdbool.h>
#ifndef INCLUDE_WATER_MANAGEMENT_WATER_READING_H_
#define INCLUDE_WATER_MANAGEMENT_WATER_READING_H_


#define N_SAMPLES 4
int index_adc, index_checks, index_reservoire;
bool block;

void adc_init(void);
void start_adc_conversion(void);
void read_reservoir_level(void);
void read_tank_level(void);


#endif /* INCLUDE_WATER_MANAGEMENT_WATER_READING_H_ */
