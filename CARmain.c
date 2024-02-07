/*
 * File:   CARmain.c
 * Author: Claudio Tomaiuolo (5630055), Teodoro Lima ()
 *
 * Created on 02 January 2024, 21:26
 */

#include <xc.h>

#include "timer_setup.h"
#include "UART_setup.h"
#include "ADC_setup.h"
#include "PWM_setup.h"
#include "RXbuffer_setup.h"
#include "TXbuffer_setup.h"
#include "task_scheduler.h"
#include "communication_protocols.h"
#include "lights_setup.h"
#include "buttons_setup.h"

//UART configuration
#define UARTone 1
#define UARTtwo 2
#define UARTONBUS1 1
#define UARTONBUS2 2

//IR configuration
#define IR_to_BUS1 0
#define IR_to_mikroBUS1 1
#define IR_to_mikroBUS2 2


int main(void) {
    ANSELA = ANSELB = ANSELC = ANSELD = ANSELE = ANSELG = 0x0000; //all pins set to digital
    INTCON2bits.GIE = 1; //set global interrupt enable
    
    UART_setup(UARTone, UARTONBUS2);
    en_ext_RXint_UART(UARTone,1);
    //en_ext_TXint_UART(UARTone,1); //enable TX interrupt for avoiding busy waiting
    ADC_setup(IR_to_mikroBUS1);
    PWM_setup();
    task_scheduler();
    protocol_setup();
    lights_setup();
    RXinitCircularArray(&RXc_buffer);
    //TXinitCircularArray(&TXc_buffer);  // Second buffer for TX for avoiding busy waiting
    
    while(1){    
        scheduler();
        tmr_wait_ms(TIMER1, 1);
    }
    return 0;
}