/* Microchip Technology Inc. and its subsidiaries.  You may use this software 
 * and any derivatives exclusively with Microchip products. 
 * 
 * THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS".  NO WARRANTIES, WHETHER 
 * EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS SOFTWARE, INCLUDING ANY IMPLIED 
 * WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY, AND FITNESS FOR A 
 * PARTICULAR PURPOSE, OR ITS INTERACTION WITH MICROCHIP PRODUCTS, COMBINATION 
 * WITH ANY OTHER PRODUCTS, OR USE IN ANY APPLICATION. 
 *
 * IN NO EVENT WILL MICROCHIP BE LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE, 
 * INCIDENTAL OR CONSEQUENTIAL LOSS, DAMAGE, COST OR EXPENSE OF ANY KIND 
 * WHATSOEVER RELATED TO THE SOFTWARE, HOWEVER CAUSED, EVEN IF MICROCHIP HAS 
 * BEEN ADVISED OF THE POSSIBILITY OR THE DAMAGES ARE FORESEEABLE.  TO THE 
 * FULLEST EXTENT ALLOWED BY LAW, MICROCHIP'S TOTAL LIABILITY ON ALL CLAIMS 
 * IN ANY WAY RELATED TO THIS SOFTWARE WILL NOT EXCEED THE AMOUNT OF FEES, IF 
 * ANY, THAT YOU HAVE PAID DIRECTLY TO MICROCHIP FOR THIS SOFTWARE.
 *
 * MICROCHIP PROVIDES THIS SOFTWARE CONDITIONALLY UPON YOUR ACCEPTANCE OF THESE 
 * TERMS. 
 */

/* 
 * File:   
 * Author: 
 * Comments:
 * Revision history: 
 */

// This is a guard condition so that contents of this file are not included
// more than once.  
#ifndef TASK_SCHEDULER_H
#define	TASK_SCHEDULER_H

#include <xc.h> // include processor files - each processor file is guarded.  
#include <math.h>
#include <stdio.h>
#include <string.h>

#include "timer_setup.h"
#include "UART_setup.h"
#include "ADC_setup.h"
#include "PWM_setup.h"
#include "RXbuffer_setup.h"
#include "TXbuffer_setup.h"
#include "communication_protocols.h"
#include "lights_setup.h"
#include "buttons_setup.h"

#define UARTone 1
#define UARTtwo 2

#define MAX_TASKS 7

#define WAIT_FOR_START 0
#define MOVING 1

#define MINTH 0.40
#define MAXTH 0.80

int state = WAIT_FOR_START;
float dist = 0, dist_min = MINTH, dist_max = MAXTH, surge = 0, yaw = 0;
char error_msg[] = "ERROR";
char buffer[25];
//int triggerTX = 0;

typedef struct {
int n;
int N;
} heartbeat;
heartbeat schedInfo[MAX_TASKS];

void task_scheduler(){
    RE8_setup(&state);
    
    for(int i=0; i <MAX_TASKS; i++){
        switch(i){
            case 0: // duty cycle
                schedInfo[i].N = 100;
                schedInfo[i].n = 0;
                break;
            case 1: // take distance with IR
                schedInfo[i].N = 1;
                schedInfo[i].n = 0;
                break;
            case 2: // battery
                schedInfo[i].N = 1000;
                schedInfo[i].n = 0;
                break;
            case 3:  //update thresholds
                schedInfo[i].N = 100; //100 = 10hz
                schedInfo[i].n = 0;
                break;             
            case 4: // control
                schedInfo[i].N = 1; //1 = 1Khz
                schedInfo[i].n = 0;
                break;
            case 5: // side lights
                schedInfo[i].N = 1000; //1000 = 1hz
                schedInfo[i].n = 0;
                break;
            case 6: // send distance with UART
                schedInfo[i].N = 100;
                schedInfo[i].n = 0;
                break;
        }
    }
}

void task_IR(){ // take distance with IR
    float volt;
    while (!AD1CON1bits.DONE){}
    volt = ((float)ADC1BUF1/(float)(MAX_INT_ADC)) * IR_VOLT;
    dist = 2.34 - 4.74 * volt + 4.06 * powf(volt,2) - 1.60 * powf(volt,3) + 0.24 * powf(volt,4);
}

void task_send_IR(){ // send IR
    sprintf(buffer,"$MDIST,%d*",(int)(dist*100));
    /*
    en_ext_TXint_UART(UARTone,0); // manage shared variable
    //TXStringEnqueue(buffer);
    en_ext_TXint_UART(UARTone,1);
     */ 
    sendString(1,buffer); //busy waiting way
}

void task_battery(){
   float volt_battery;
   volt_battery = ((float)ADC1BUF0/(float)(MAX_INT_ADC)) * BATTERY_VOLT;
   sprintf(buffer,"$MBATT,%.2f*",(double)(3*volt_battery));
   /*
   en_ext_TXint_UART(UARTone,0); // manage shared variable
   TXStringEnqueue(buffer);
   en_ext_TXint_UART(UARTone,1);
    */
   sendString(1,buffer); //busy waiting way
}

void task_threshold_update(){ //update thresholds
    en_ext_RXint_UART(UARTone,0); //manage shared variables turning the RX interrupt OFF
    char data_rcv;
    int ret, val_min, val_max; 
    while(!RXisEmpty(&RXc_buffer)){
        data_rcv = RXdequeue(&RXc_buffer);
        ret = parse_byte(&pstate, data_rcv);
        if (ret == NEW_MESSAGE){
            if (strcmp(pstate.msg_type, MSG_INFO) == 0) {
                sscanf(pstate.msg_payload,"%d,%d",&val_min,&val_max);
                dist_min = (float)val_min/100.0;
                dist_max = (float)val_max/100.0;
            } else if (ret == -1){
                /*
                en_ext_TXint_UART(UARTone,0); // manage shared variable
                TXStringEnqueue(error_msg); // send 'ERROR' to UART 1
                en_ext_TXint_UART(UARTone,1);
                 */
                sendString(1,error_msg); //busy waiting way
            }
        }
    }
    en_ext_RXint_UART(UARTone,1); //manage shared variables turning the RX interrupt ON
}

void task_control(){
    if(state == WAIT_FOR_START){
        surge = 0;
        yaw = 0;
        turnoff_motors();
    } else if(state == MOVING){
        if(dist > dist_max){
            surge = 1;
            yaw = 0;
        } else if(dist <= dist_max && dist >= dist_min){
            surge = (dist - dist_min) / (dist_max - dist_min); //normalizing (more surge goes to 1, more yaw goes to 0)
            yaw = 1-surge;
        } else if(dist < dist_min){
            surge = 0;
            yaw = 1;
        }
        move(surge + yaw, surge - yaw);
    }
}

void task_motors(){ // send duty cycles to UART
   sprintf(buffer,"$MPWM,%d,%d,%d,%d*",ocr_data.oc1r, ocr_data.oc2r, ocr_data.oc3r, ocr_data.oc4r);
   
   /*
   if (triggerTX == 0){
       sendString(1, buffer); //trigger the first transfer
       triggerTX++;
   } else {
        en_ext_TXint_UART(UARTone,0); // manage shared variable
        //TXStringEnqueue(buffer);
        en_ext_TXint_UART(UARTone,1);
   }
    */
   sendString(1,buffer); //busy waiting way
}

void scheduler() {
    for (int i = 0; i < MAX_TASKS; i++) {
        schedInfo[i].n++;
        if(schedInfo[i].n >= schedInfo[i].N){
            switch(i){
                case 0:
                    task_motors();
                    break;
                case 1:
                    task_IR();
                    break;
                case 2:
                    task_battery();
                    break;
                case 3:
                    task_threshold_update();
                    break;
                case 4:
                    task_control();
                    head_rear_lights(state, surge);
                    break;
                case 5:
                    side_lights(state, yaw);
                    break;
                case 6:
                    task_send_IR();
                    break;
            }
            schedInfo[i].n = 0;
        }
    }
}

#ifdef	__cplusplus
extern "C" {
#endif /* __cplusplus */

    // TODO If C++ is being used, regular C code needs function names to have C 
    // linkage so the functions can be used by the c code. 

#ifdef	__cplusplus
}
#endif /* __cplusplus */

#endif	/* XC_HEADER_TEMPLATE_H */

