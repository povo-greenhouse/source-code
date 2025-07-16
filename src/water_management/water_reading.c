///*
// * water_reading.c
// *
// *  Created on: 24 giu 2025
// *      Author: vince
// */
//
#include "water_management/water_reading.h"
#include "water_management/pump_management.h"
#include "water_management/water_init.h"
#include "uart_communication/uart_comm.h"

#include "IOT/IOT_communication.h"
#include "scheduling/scheduler.h"
#include <stdbool.h>
#include <stdio.h>

void read_reservoire(){
    uint32_t res_value=water_arr[0];
    if(res_value<=water_option_values.reservoire_empty_threshold){
        send_data(4, 1, 4);
        printf("Reservoire empty \n");
    }
    else if (res_value>=water_option_values.reservoire_empty_threshold && res_value<water_option_values.reservoire_low_threshold){
        send_data(4, 1, 3)  ;
        printf("Low water in the Reservoir \n");
    }
    else if (res_value>=water_option_values.reservoire_low_threshold && res_value<water_option_values.reservoire_moderate_threshold){
        send_data(4, 0, 2);
        printf("Moderate water  in the Reservoir \n");
    }
    else if (res_value>=water_option_values.reservoire_moderate_threshold ){
        send_data(4, 0, 1);
        printf("Reservoire full \n");
    }
    printf("Reading executed! \n");
}


void read_tank(){
    uint32_t tank_value=water_arr[1];
    if(tank_value>water_option_values.tank_empty_threshold){
        P4->OUT &= ~BIT7;
        send_data(3, 1, 0);
        block = true;
        printf("Tank full! pumps have been blocked");
    }else{
        block=false;
        send_data(3, 0, 0);
        printf("Tank empty! good to go");
    }
}


void upd_res_empty_threshold(int32_t val);
void upd_res_low_threshold(int32_t val);
void upd_res_moderate_threshold(int32_t val);
void upd_tank_empty_threshold(int32_t val);
void upd_tank_read_time(int32_t val);
void upd_res_read_time(int32_t val);
void add_water_reading_options(){
    int error;
    OptionUnion res_empty_thresh = option_u_new_threshold(RESERVOIRE_EMPTY_THRESH_DEFAULT,0,20,1,&error);

    option_menu_push_option(option_new("Empty Reservoire Threshold",
                                                           THRESHOLD,
                                                           res_empty_thresh,
                                             upd_res_empty_threshold,
                                         to_string_threshold_default));
    OptionUnion res_low_thresh = option_u_new_threshold(RESERVOIRE_LOW_THRESH_DEFAULT,0,500,2,&error);


    option_menu_push_option(option_new("Low Reservoire Threshold",
                                                           THRESHOLD,
                                                           res_low_thresh,
                                             upd_res_low_threshold,
                                         to_string_threshold_default));

    OptionUnion res_moderate_thresh = option_u_new_threshold(RESERVOIRE_MODERATE_THRESH_DEFAULT,0,500,2,&error);


    option_menu_push_option(option_new("Moderate Reservoire Threshold",
                                                           THRESHOLD,
                                                           res_moderate_thresh,
                                             upd_res_moderate_threshold,
                                         to_string_threshold_default));

    OptionUnion tank_empty_thresh = option_u_new_threshold(TANK_EMPTY_THRESH_DEFAULT,0,500,2,&error);


    option_menu_push_option(option_new("Empty Tank Threshold",
                                                           THRESHOLD,
                                                           tank_empty_thresh,
                                             upd_tank_empty_threshold,
                                         to_string_threshold_default));


    OptionUnion read_tank_time = option_u_new_timer(timer_option_get_name_by_value(READ_TANK_TIME_DEFAULT),&error);

    option_menu_push_option(option_new("Tank Reading Time",
                                                     TIMER,
                                                     read_tank_time,
                                                     upd_tank_read_time,
                                                     to_string_timer_default));
    OptionUnion read_reservoire_time = option_u_new_timer(timer_option_get_name_by_value(READ_RESERVOIRE_TIME_DEFAULT),&error);

    option_menu_push_option(option_new("Reservoire Reading Time",
                                                           TIMER,
                                                           read_reservoire_time,
                                                           upd_res_read_time,
                                                           to_string_timer_default
                                       ));


}

void upd_res_empty_threshold(int32_t val){
    water_option_values.reservoire_empty_threshold = val;
}
void upd_res_low_threshold(int32_t val){
    water_option_values.reservoire_low_threshold = val;
}
void upd_res_moderate_threshold(int32_t val){
    water_option_values.reservoire_moderate_threshold = val;
}
void upd_tank_empty_threshold(int32_t val){
    water_option_values.tank_empty_threshold = val;
}

void upd_tank_read_time(int32_t val){
    water_option_values.read_tank_time=val;
    task_list.task_array[index_tank].max_time = val;
}
void upd_res_read_time(int32_t val){
    water_option_values.read_reservoire_time = val;
    task_list.task_array[index_reservoire].max_time=val;

}
// _________________________OLD CODE USING ADC 14___________________________________________________
//
//int old_value=0,old_value_1 = 0 ,limit=0, first_value=0,first_value_1 = 0;;
//bool first_reading=true,first_reading_1 = true;
//
//
// int sum_1 = 0, sum_2 = 0;
// int count_1 = 0, count_2 = 0;
//
//void adc_init(void) {
//
//    P5->SEL0 |= BIT5 | BIT4;
//    P5->SEL1 |= BIT5 | BIT4;
//
//
//    ADC14->CTL0 &= ~ADC14_CTL0_ENC;
//    ADC14->CTL0 = ADC14_CTL0_SHT0_5| ADC14_CTL0_ON | ADC14_CTL0_SHP |
//                  ADC14_CTL0_SSEL__MCLK | ADC14_CTL0_CONSEQ_3;
//
//    ADC14->CTL1 = ADC14_CTL1_RES_3;
//
//    ADC14->MCTL[0] = ADC14_MCTLN_INCH_0;
//    ADC14->MCTL[1] = ADC14_MCTLN_INCH_1 | ADC14_MCTLN_EOS;
//
//    ADC14->IER0 = ADC14_IER0_IE0 | ADC14_IER0_IE1;
//
//    NVIC->ISER[0] = 1 << ((ADC14_IRQn) & 31);
//
//    ADC14->CTL0 |= ADC14_CTL0_ENC;
//
//    __enable_irq();
//
//
//}
//
//
//void start_adc_conversion(){
//
//        ADC14->CTL0 |= ADC14_CTL0_SC;
//        disable_task_at(index_adc);
//}
//
//
//void ADC14_IRQHandler(void) {
//    if (ADC14->IFGR0 & ADC14_IFGR0_IFG0) {
//        enable_task_at(index_checks);
//        ADC14->CLRIFGR0 = ADC14_CLRIFGR0_CLRIFG0;
//    }
//    if (ADC14->IFGR0 & ADC14_IFGR0_IFG1) {
//        enable_task_at(index_reservoire);
//        ADC14->CLRIFGR0 = ADC14_CLRIFGR0_CLRIFG1;
//    }
//
//
//}
//
//
//

//void read_tank_level() {
//
//    int value = ADC14->MEM[0];
//    sum_1 += value;
//    count_1++;
//
//    if (count_1 < 15) {
//        enable_task_at(index_adc);    // richiedi nuova lettura alla prossima chiamata
//        disable_task_at(index_checks);
//    }else{
//        value = sum_1 / 15;    // media su 15 letture
//
//           if (first_reading_1) {
//               first_value_1 = value;
//               first_reading_1 = false;
//           } else {
//               int diff =value-first_value_1;
//               printf("value: %d, old: %d, diff: %d\n", value, first_value_1, diff);
//
//               // Se la differenza supera la soglia, spegni GPIO e manda dati
//               if (diff > 8) {
//                   // Esempio: spegni BIT7 (modifica secondo la tua logica)
//                   P4->OUT &= ~BIT7;
//                   send_data(3, 1, 0);
//                   block = true;
//                   limit = diff;
//               }
//
//               if (limit>diff){
//                   send_data(3, 0, 0);
//                   block=false;
//                   limit=0;
//               }
//           }
//
//           // Reset counter e sum per la prossima media
//           sum_1 = 0;
//           count_1 = 0;
//
//           enable_task_at(index_adc);
//           disable_task_at(index_checks);
//    }
//
//}
//
//
//
//void read_reservoir_level(){
//
//
//    int value = ADC14->MEM[1];
//    sum_2 += value;
//    count_2++;
//
//    if(act_1){
//           value+=15;
//     }
//     if(act_2){
//          value+=15;
//      }
//
//    if (count_2 < 15) {
//        enable_task_at(index_adc);        // richiedi nuova lettura alla prossima chiamata
//        disable_task_at(index_reservoire);
//
//    }else{
//        // Quando count == 8
//            value = sum_2 / 15;    // media su 8 letture
//
//            if (first_reading) {
//                first_value = value;
//                first_reading = false;
//            } else {
//                int diff = first_value - value;
//                printf("value: %d, first_reading: %d, diff: %d\n", value, first_value, diff);
//                if (diff >= 1 && diff < 10) {
//                    send_data(4, 0, 1);
//                    printf("1\n");
//                } else if (diff >= 10 && diff < 20) {
//                    send_data(4, 0, 2);
//                    printf("2\n");
//                } else if (diff >= 20 && diff < 30) {
//                    send_data(4, 1, 3);
//                    printf("3\n");
//                } else if (diff >= 30) {
//                    send_data(4, 1, 4);
//                    printf("4\n");
//                }
//            }
//
//            // Reset counter e sum per la prossima media
//            sum_2 = 0;
//            count_2 = 0;
//
//            enable_task_at(index_adc);
//    }
//
//
//    disable_task_at(index_reservoire);
//}
//
