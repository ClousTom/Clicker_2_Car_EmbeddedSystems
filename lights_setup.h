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
#ifndef LIGHTS_SETUP_H
#define	LIGHTS_SETUP_H

#include <xc.h>

#define WAIT_FOR_START 0
#define MOVING 1

void lights_setup(){
    TRISFbits.TRISF1 = 0; 
    LATFbits.LATF1 = 0; // right side lights
    
    TRISBbits.TRISB8 = 0; 
    LATBbits.LATB8 = 0; // left side lights
    
    TRISAbits.TRISA7 = 0;
    LATAbits.LATA7 = 0; // beam head light
    
    TRISGbits.TRISG1 = 0; 
    LATGbits.LATG1 = 0; // low intensity lights
    
    TRISFbits.TRISF0 = 0; 
    LATFbits.LATF0 = 0; // brakes
}


void head_rear_lights(int state, float surge){ //set rear and head lights
    if (state == MOVING) {
        if (surge > 0.5) {
            LATAbits.LATA7 = 1; // beam head lights
            LATGbits.LATG1 = 0; // low intensity lights
            LATFbits.LATF0 = 0; // brakes
        } else if (surge < 0.5){
            LATAbits.LATA7 = 0; // beam head lights
            LATGbits.LATG1 = 1; // low intensity head lights
            LATFbits.LATF0 = 1; // brakes
        }
    } else { // WAIT FOR START state
        LATAbits.LATA7 = 0; // beam head lights
        LATGbits.LATG1 = 0; // low intensity lights
        LATFbits.LATF0 = 0; // brakes
    }
}

void side_lights(int state, float yaw){ //function to set the state of side lights
    if (state == MOVING) {
        if (yaw > 0.15) {
            LATBbits.LATB8 = 0; // left side light
            LATFbits.LATF1 ^= 1; // right side light
        } else if (yaw < 0.15) {
            LATBbits.LATB8 = 0; // left side light
            LATFbits.LATF1 = 0; // right side light
        }
    } else {
        LATBbits.LATB8 = !PORTBbits.RB8; // toggle left side light
        LATFbits.LATF1 = !PORTBbits.RB8; // toggle right side light
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

