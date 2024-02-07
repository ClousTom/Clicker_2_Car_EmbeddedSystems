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
#ifndef PWM_SETUP_H
#define	PWM_SETUP_H

#include <xc.h> // include processor files - each processor file is guarded.  

#include "timer_setup.h"

#define FPWM 10000
#define OCRS FCY/FPWM // OCXRS=FCY/10 KHz

typedef struct{
    int oc1r;
    int oc2r;
    int oc3r;
    int oc4r;
} PWMinfo; // used for sending information about duty cycle through UART

PWMinfo ocr_data;

void PWM_setup(){
    RPOR0bits.RP65R = 0x10;  // mapping pin rd1-rd2-rd3-rd4
    RPOR1bits.RP66R = 0x11;
    RPOR1bits.RP67R = 0x12;
    RPOR2bits.RP68R = 0x13;
    
    OC1CON1bits.OCTSEL = 7;    // peripheral clock used as source for output compare
    OC1CON2bits.SYNCSEL = 0x1F; // no sync required   Tpwm = Tcy
    OC1CON1bits.OCM = 6;       // edge align PWM
    
    OC2CON1bits.OCTSEL = 7;    // peripheral clock used as source for output compare
    OC2CON2bits.SYNCSEL = 0x1F; // no sync required   Tpwm = Tcy
    OC2CON1bits.OCM = 6;       // edge align PWM
    
    OC3CON1bits.OCTSEL = 7;    // peripheral clock used as source for output compare
    OC3CON2bits.SYNCSEL = 0x1F; // no sync required   Tpwm = Tcy
    OC3CON1bits.OCM = 6;       // edge align PWM
    
    OC4CON1bits.OCTSEL = 7;    // peripheral clock used as source for output compare
    OC4CON2bits.SYNCSEL = 0x1F; // no sync required   Tpwm = Tcy
    OC4CON1bits.OCM = 6;       // edge align PWM

    OC1RS = OC2RS = OC3RS = OC4RS = OCRS; // pwm frequence = 10KHz  
    OC1R = 0;
    OC2R = 0; 
    OC3R = 0;
    OC4R = 0;
    ocr_data.oc1r = (int)(((float)OC1R/(float)OC1RS)*100);
    ocr_data.oc2r = (int)(((float)OC2R/(float)OC2RS)*100);
    ocr_data.oc3r = (int)(((float)OC3R/(float)OC3RS)*100);
    ocr_data.oc4r = (int)(((float)OC4R/(float)OC4RS)*100);
}

void turnoff_motors(){
    OC1R = 0; // reset all duty cycles to zero
    OC2R = 0; 
    OC3R = 0;
    OC4R = 0; 
    ocr_data.oc1r = (int)(((float)OC1R/(float)OC1RS)*100);
    ocr_data.oc2r = (int)(((float)OC2R/(float)OC2RS)*100);
    ocr_data.oc3r = (int)(((float)OC3R/(float)OC3RS)*100);
    ocr_data.oc4r = (int)(((float)OC4R/(float)OC4RS)*100);
}

void move(float ctrl_left, float ctrl_right) {
    if (ctrl_left >= 0) { // assign motor direction depending on control signal sign
        OC1R = 0;
        OC2R = (unsigned int)(ctrl_left * OCRS);
    } else {
        OC1R = (unsigned int)(-ctrl_left * OCRS);
        OC2R = 0;
    }
    if (ctrl_right >= 0) { // assign motor direction depending on control signal sign
        OC3R = 0;
        OC4R = (unsigned int)(ctrl_right * OCRS);
    } else {
        OC3R = (unsigned int)(-ctrl_right * OCRS);
        OC4R = 0;
    }
    ocr_data.oc1r = (int)(((float)OC1R/(float)OC1RS)*100);
    ocr_data.oc2r = (int)(((float)OC2R/(float)OC2RS)*100);
    ocr_data.oc3r = (int)(((float)OC3R/(float)OC3RS)*100);
    ocr_data.oc4r = (int)(((float)OC4R/(float)OC4RS)*100);
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

