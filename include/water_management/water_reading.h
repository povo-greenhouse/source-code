/*
 * water_reading.h
 *
 *  Created on: 24 giu 2025
 *      Author: vince
 */
#include <stdbool.h>
#ifndef INCLUDE_WATER_MANAGEMENT_WATER_READING_H_
#define INCLUDE_WATER_MANAGEMENT_WATER_READING_H_


//#define N_SAMPLES 4
int index_tank, index_reservoire;
bool block;

//void adc_init(void);
#define RESERVOIRE_TRESH_DEFAULT 25


#define RESERVOIRE_EMPTY_THRESH_DEFAULT 10
#define RESERVOIRE_LOW_THRESH_DEFAULT 80
#define RESERVOIRE_MODERATE_THRESH_DEFAULT 150
#define TANK_EMPTY_THRESH_DEFAULT 200

#define READ_TANK_TIME_DEFAULT 2000
#define READ_RESERVOIRE_TIME_DEFAULT 2000



void read_tank(void);
void read_reservoire(void);

void add_water_reading_options();


#endif /* INCLUDE_WATER_MANAGEMENT_WATER_READING_H_ */
