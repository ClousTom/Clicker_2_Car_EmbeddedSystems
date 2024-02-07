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
#ifndef BUTTONS_SETUP_H
#define	BUTTONS_SETUP_H

#include <xc.h>

#include "timer_setup.h"
#include "task_scheduler.h"

#define WAIT_FOR_START 0
#define MOVING 1

int *state_for_button; // pointer for the system state

void RE8_setup(int *state1){
    state_for_button = state1;

    RPINR0bits.INT1R = 0x58; // set interrupt pin (0x58 is 88)
    INTCON2bits.INT1EP = 1; // interrupt on negative edge 
    IEC1bits.INT1IE = 1; // enable external interrupt INT1
    en_ext_int_timer(TIMER2, 1); //enable external interrupt for TIMER 2 
}

void __attribute__ ((__interrupt__ , __auto_psv__)) _INT1Interrupt() { //buttorn RE8 interrupt
    IEC1bits.INT1IE = 0; // disable interrupt button
    tmr_setup_period(TIMER2,20);  //timer 2 for debouncing
}

void __attribute__ ((__interrupt__ , __auto_psv__)) _T2Interrupt() { // for button bouncing
    T2CONbits.TON = 0; // disable timer 2
    IFS0bits.T2IF = 0; // reset interrupt flag
    
    if(PORTEbits.RE8 == 0){    // check button status
        if (*state_for_button == MOVING){
            *state_for_button = WAIT_FOR_START;
        } else {
            *state_for_button = MOVING;
        }
    } else {
        TMR2 = 0; // reset TIMER2
        T2CONbits.TON = 1; // start TIMER2
    }
    IFS1bits.INT1IF = 0; // POSSIBILE ERRORE (queste due dovevano stare nel primo if)
    IEC1bits.INT1IE = 1; // able interrupt firing for button
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

