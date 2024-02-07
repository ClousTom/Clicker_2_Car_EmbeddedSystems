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

#ifndef TIMER_SETUP_H
#define	TIMER_SETUP_H

#include <xc.h> 

#define TIMER1 1
#define TIMER2 2
#define TIMER3 3
#define TIMER4 4
#define TIMER5 5
#define FOSC  144000000 //frequency 144Mhz
#define FCY (FOSC/2)
#define MAXn 65535 //16bit timer
#define MaxTime 200 //period cycles 

float loops = 0.0;

void configure_bit(int timer, int ms, int prescale, int prescale_binary){
    switch(timer){
        case TIMER1:
            TMR1 = 0; // reset timer
            PR1 = (uint16_t)((float)ms / 1000.0 * FCY / (float)prescale); // set period register
            T1CONbits.TON = 0;  // disable timer
            T1CONbits.TCKPS = prescale_binary; // set prescale
            T1CONbits.TCS = 0;  // external clock
            T1CONbits.TGATE = 0;
            IFS0bits.T1IF = 0; // reset interrupt timer 1
            T1CONbits.TON = 1;
            break;
        case TIMER2:
            TMR2 = 0;// reset timer
            PR2 = (uint16_t)((float)ms / 1000.0 * FCY / (float)prescale); // set period register
            T2CONbits.TON = 0;  // disable timer
            T2CONbits.TCKPS = prescale_binary; // set prescale
            T2CONbits.TCS = 0;  // external clock
            T2CONbits.TGATE = 0;
            IFS0bits.T2IF = 0; // reset interrupt timer 2
            T2CONbits.TON = 1;
            break;
        
        case TIMER3:
            TMR3 = 0; // reset timer
            PR3 = (uint16_t)((float)ms / 1000.0 * FCY / (float)prescale); // set period register
            T3CONbits.TON = 0;  // disable timer
            T3CONbits.TCKPS = prescale_binary; // set prescale
            T3CONbits.TCS = 0;  // external clock
            T3CONbits.TGATE = 0;
            IFS0bits.T3IF = 0; // reset interrupt timer 3
            T3CONbits.TON = 1;
            break;
    }
}

float tmr_setup_period(int timer, int ms) {
    float prescale_f;
    int prescale, prescale_binary;
    if(ms <= MaxTime){
        prescale_f = (float)ms / 1000.0 * FCY / (float)MAXn;
    if (prescale_f <= 1){
        prescale = 1;
        prescale_binary = 0;
    }
    else if (prescale_f <= 8){
        prescale = 8;
        prescale_binary = 1;
    }
    else if (prescale_f <= 64){
        prescale = 64;
        prescale_binary = 2;
    }
    else if (prescale_f <= 256){
        prescale = 256;
        prescale_binary = 3;
    }
    configure_bit(timer, ms, prescale, prescale_binary);  
   } else{
        prescale = 256;
        prescale_binary = 3;
        loops = (float)ms / MaxTime;
        configure_bit(timer, MaxTime, prescale, prescale_binary);
    }
}

void tmr_wait_period(int timer){
    switch(timer){
        case 1:  
            while(IFS0bits.T1IF==0){};
            IFS0bits.T1IF=0;
            T1CONbits.TON = 0x00; 
            break;
        case 2:
            while(IFS0bits.T2IF==0){};
            IFS0bits.T2IF=0;
            T2CONbits.TON = 0x00; 
            break;
    } 
}

void tmr_wait_ms(int timer, int ms){
    tmr_setup_period(timer, ms);
    tmr_wait_period(timer);
}

void setup_period(int timer, int ms){ //remember to shot down the timer manually
    int loops = ms/MaxTime; //global variable for manage counters over 200ms
    tmr_setup_period(timer, MaxTime);
}

void wait_ms(int timer, int ms){
    int timerloop = ms / MaxTime;
    for (int i=0; i<timerloop; i++){
        tmr_wait_ms(timer, MaxTime);
    }
    if (ms % MaxTime != 0){
        int rest = ms % MaxTime;
        tmr_wait_ms(timer, rest);
    }
}

void en_ext_int_timer(int timer, int enabler){
    switch(timer){
        case 1:
            if(enabler==1){
                IEC0bits.T1IE = 1; // enable interrupt TIMER 1
            }else{
                IEC0bits.T1IE = 0; // disable interrupt TIMER 1
            }
            break;
        case 2:
            if(enabler==1){
                IEC0bits.T2IE = 1; // enable interrupt TIMER 2
            }else{
                IEC0bits.T2IE = 0; // disable interrupt TIMER 2
            }
            break;
        case 3:
            if(enabler==1){
                IEC0bits.T3IE = 1; // enable interrupt TIMER 3
            }else{
                IEC0bits.T3IE = 0; // disable interrupt TIMER 3
            }
            break;
    }
}

#ifdef	__cplusplus
extern "C" {
#endif /* __cplusplus */

#ifdef	__cplusplus
}
#endif /* __cplusplus */

#endif	/* XC_HEADER_TEMPLATE_H */

