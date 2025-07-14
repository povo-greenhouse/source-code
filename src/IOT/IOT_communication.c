/*
 * IOT_communication.c
 *
 *  Created on: 25 giu 2025
 *      Author: vince
 */

#include "IOT/IOT_communication.h"
#include "msp.h"

void init_GPIOs_IOT(){
            P2->SEL0 &= ~BIT3;//bit 9 port 11  Set GPIO
            P2->SEL1 &= ~BIT3;              // Set GPIO
            P2->DIR  |= BIT3;               // Set as output
            P2->OUT|=BIT3;                  // Set 1

            P5->SEL0 &= ~BIT1;//bit 8 port 10
            P5->SEL1 &= ~BIT1;
            P5->DIR  |= BIT1;
            P5->OUT|=BIT1;

            P3->SEL0 &= ~BIT5;//bit 7 port 9
            P3->SEL1 &= ~BIT5;
            P3->DIR  |= BIT5;
            P3->OUT|=BIT5;

            P3->SEL0 &= ~BIT7;//bit 6 port 8
            P3->SEL1 &= ~BIT7;
            P3->DIR  |= BIT7;
            P3->OUT|=BIT7;


            P5->SEL0 &= ~BIT7; //bit 5 port 7
            P5->SEL1 &= ~BIT7;
            P5->DIR  |= BIT7;
            P5->OUT|=BIT7;

            P1->SEL0 &= ~BIT6;//bit 4 port 6
            P1->SEL1 &= ~BIT6;
            P1->DIR  |= BIT6;
            P1->OUT|=BIT6;


            P1->SEL0 &= ~BIT7;//bit 3 port 5
            P1->SEL1 &= ~BIT7;
            P1->DIR  |= BIT7;
            P1->OUT|=BIT7;

            P5->SEL0 &= ~BIT0;//BIT 2 port 4
            P5->SEL1 &= ~BIT0;
            P5->DIR  |= BIT0;
            P5->OUT|=BIT0;

            P5->SEL0 &= ~BIT2;//bit 1 port 3
            P5->SEL1 &= ~BIT2;
            P5->DIR  |= BIT2;
            P5->OUT|=BIT2;

            P3->SEL0 &= ~BIT6;//bit 0 port 2 (select)
            P3->SEL1 &= ~BIT6;
            P3->DIR  |= BIT6;
            P3->OUT|=BIT6;
}

void send_data(int code, bool active, int value){
    P3->OUT |= BIT6;
    switch(code){
                case 1: // 110 AIR
                    P2->OUT |=  BIT3;
                    P5->OUT |=  BIT1;
                    P3->OUT &= ~BIT5;
                    break;
                case 2: // 101 TEMP
                    P2->OUT |=  BIT3;
                    P5->OUT &= ~BIT1;
                    P3->OUT |=  BIT5;
                    break;
                case 3: // 100  WATER TANK
                    P2->OUT |=  BIT3;
                    P5->OUT &= ~BIT1;
                    P3->OUT &= ~BIT5;
                    break;
                case 4: // 011  RESERVOIRE
                    P2->OUT &= ~BIT3;
                    P5->OUT |=  BIT1;
                    P3->OUT |=  BIT5;
                    break;
                case 5: // 010  LIGHT
                    P2->OUT &= ~BIT3;
                    P5->OUT |=  BIT1;
                    P3->OUT &= ~BIT5;
                    break;
                case 6: // 001  PUMP 1
                    P2->OUT &= ~BIT3;
                    P5->OUT &= ~BIT1;
                    P3->OUT |=  BIT5;
                    break;
                case 7: // 000  PUMP 2
                    P2->OUT &= ~BIT3;
                    P5->OUT &= ~BIT1;
                    P3->OUT &= ~BIT5;
                    break;
            }
            if (active)
                P3->OUT &= ~BIT7;
            else
                P3->OUT |= BIT7;

            switch(value){
                case 1:

                    P5->OUT |= BIT7;
                    P3->OUT |= BIT0;
                    break;

                case 2:

                    P5->OUT |= BIT7;
                    P3->OUT &= ~BIT0;
                    break;

                case 3:

                    P5->OUT &= ~BIT7;
                    P3->OUT |= BIT0;
                    break;

                case 4:

                    P5->OUT &= ~BIT7;
                    P3->OUT &= ~BIT0;
                    break;

                default: break;
            }
            P3->OUT &= ~BIT6;
            init_GPIOs_IOT();
}
