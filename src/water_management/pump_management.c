/*
 * PumpManagement.c
 *
 *  Created on: 24 mag 2025
 *      Author: vince
 */

#include "water_management/pump_management.h"
#include "water_management/water_reading.h"
#include <stdio.h>
#include <stdbool.h>

//1.5 pump 1
//4.6 pump 2
//5.4 sensor 1
//5.5 sensor 2



void pump_init() {
    printf("Executing: pump_init()\n");
    P6->DIR |= BIT7;
    P6->OUT &= ~BIT7;

    P6->DIR |= BIT6;
    P6->OUT &= ~BIT6;


}

void activatePump1() {
    if (!block){
        printf("Executing: activatePump1()\n");
        send_data(6, 1, 0);

        P6->OUT |= BIT6; // Modificato da P1->OUT |= BIT5

        disable_task_at(activate_pump1_index);
        enable_task_at(deactivate_pump1_index);
        act_1=true;
    }else{
        printf("Blocked pump 1 act");
    }
}


void turnOffPump1() {
    if(!block){
        printf("Executing: turnOffPump1()\n");
        send_data(6, 0, 0);

        P6->OUT &= ~BIT6;

        disable_task_at(deactivate_pump1_index);
        enable_task_at(activate_pump1_index);
        act_1=false;
    }else{
        printf("Blocked pump 1 off");
    }
}

void activatePump2() {

        printf("Executing: activatePump2()\n");
        send_data(7, 1, 0);
        P6->OUT |= BIT7;

        disable_task_at(activate_pump2_index);
        enable_task_at(deactivate_pump2_index);
        act_2=true;
}

void turnOffPump2() {
    if (!block){
        printf("Executing: turnOffPump2()\n");
        send_data(7, 0, 0);
        P6->OUT &= ~BIT7;
        disable_task_at(deactivate_pump2_index);
        enable_task_at(activate_pump2_index);
        act_2=false;
    }else{
        printf("Blocked pump 2 turnoff");
    }


}
