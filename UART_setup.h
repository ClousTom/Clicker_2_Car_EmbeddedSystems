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

#ifndef UART_SETUP_H
#define	UART_SETUP_H

#include <xc.h>
#include <string.h>

#include "timer_setup.h"
#include "RXbuffer_setup.h"
#include "TXbuffer_setup.h"

#define BAUD 9600 // change to 38400 %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

void UART_setup(int uart, int bus){
    // case 1: enable UART1 mapped to pins RD0 and RD11
    // case 2: enable UART2 mapped to pins RE5 and RE6
    switch(uart){
        case 1:
            if(bus==1){ // set UART1 to BUS 1
                RPOR6bits.RP85R = 0x01;
                RPINR18bits.U1RXR = 0x56;
            } else if(bus==2){ // set UART1 to BUS 2
                RPOR0bits.RP64R = 0x01; // Map UART 1 TX to pin RD0 which is RP64
                RPINR18bits.U1RXR = 0x4B; // Map UART 1 RX to pin RD11 which is RPI75 (0x4B = 75)
            }
            U1BRG = (long)FCY/((long)16*(long)BAUD)-(long)1;
            U1MODEbits.UARTEN = 1; // enable UART1
            U1STAbits.UTXEN = 1; // enable U1TX (must be after UARTEN)
            U1STAbits.URXISEL = 0; // generate interrupt everytime a character is received
            //U1STAbits.UTXISEL0 = 0x00; //interrupt when at least a FIFO cell is empty
            //U1STAbits.UTXISEL1 = 0x00; 
            break;
        case 2:
            if(bus==1){  // Set UART2 to BUS 1
                RPOR6bits.RP85R = 0x03; // Map UART 2 TX to pin RE5 which is RP85
                RPINR19bits.U2RXR = 0x56; // Map UART 2 RX to pin RE6 which is RPI86 (0x56 = 86)
            } else if(bus==2){  // Set UART2 to BUS 2
                RPOR0bits.RP64R = 0x03;
                RPINR19bits.U2RXR = 0x4B;
            }
            U2BRG = (long)FCY/((long)16*(long)BAUD)-(long)1;
            U2MODEbits.UARTEN = 1; // enable UART2
            U2STAbits.UTXEN = 1; // enable U1TX (must be after UARTEN)
            U2STAbits.URXISEL = 0; // generate interrupt everytime a character is received
    }
}

void en_ext_RXint_UART(int uart, int enabler){
    switch(uart){
        case 1:
            if(enabler==1){
                IEC0bits.U1RXIE = 1; // enable uart 1 RX interrupt
            }else{
                IEC0bits.U1RXIE = 0; // disable uart 1 RX interrupt
            }
            break;
        case 2:
            if(enabler==1){
                IEC1bits.U2RXIE = 1; // enable uart 2 RX interrupt
            }else{
                IEC1bits.U2RXIE = 1; // enable uart 2 RX interrupt
            }
            break;
    }
}
/*
void en_ext_TXint_UART(int uart, int enabler){
        switch(uart){
        case 1:
            if(enabler==1){
                IEC0bits.U1TXIE = 1; // enable uart 1 RX interupt
            }else{
                IEC0bits.U1RXIE = 0; // disable uart 1 RX interupt
            }
            break;
        case 2:
            if(enabler==1){
                IEC1bits.U2RXIE = 1; // enable uart 2 RX interupt
            }else{
                IEC1bits.U2RXIE = 1; // enable uart 2 RX interupt
            }
            break;
    }
}*/

void sendData(int uart, char data) { //send data to the UART
    switch (uart) {
        case 1:
            U1TXREG = data;
            break;
        case 2:
            U2TXREG = data;
            break;
    }
}

void sendString(int uart, char* str){ //send one by one the chars of a string to the UART
    int i = 0;
    int len = strlen(str);
    switch(uart){
        case 1:
            while(i < len){
                while(U1STAbits.UTXBF == 0){  // transmit buffer is not full
                    U1TXREG = str[i];
                    i++;
                }
            }
            break;
        case 2:
            while(i < len){
                while(U2STAbits.UTXBF == 0){  // transmit buffer is not full
                    U2TXREG = str[i];
                    i++;
                }
            }
            break;
    }
}

void __attribute__ ((__interrupt__ , __auto_psv__)) _U1RXInterrupt() {
    IFS0bits.U1RXIF = 0; // reset interrupt flag
    RXenqueue(&RXc_buffer,(char)U1RXREG);  // store received character into circular buffer
}

void __attribute__ ((__interrupt__ , __auto_psv__)) _U1TXInterrupt() { //send data to UART without BUSY WAITING
    IFS0bits.U1TXIF = 0; // reset interrupt flag
    if (!TXisEmpty(&TXc_buffer)){
        char cdata = TXdequeue(&TXc_buffer);
        sendData(1, cdata);
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

