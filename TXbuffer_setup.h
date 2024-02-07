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
#ifndef TXBUFFER_SETUP_H
#define	TXBUFFER_SETUP_H

#include <xc.h> 

#define SIZE_OF_TXBUFFER 130 // round((9600(baud rate)/10(signals for a byte))/10(program running at 10Hz)) + 20% more or less
typedef struct { 
    char array[SIZE_OF_TXBUFFER];
    int front, rear;
} TXCircularArray;

TXCircularArray TXc_buffer; // declaration of a Circular Array

void TXinitCircularArray(TXCircularArray *circularArray) { // Initialization of an empty Circular Array
    circularArray->front = -1;
    circularArray->rear = -1;
}

int TXisFull(TXCircularArray *circularArray){
    return (circularArray->front == 0 && circularArray->rear == SIZE_OF_TXBUFFER - 1) || (circularArray->front == circularArray->rear + 1);
}

int TXisEmpty(TXCircularArray *circularArray){
    return circularArray->front == -1;
}

void TXenqueue(TXCircularArray *circularArray, char data) { //Add data to the circular array
    if (!TXisFull(circularArray)) {
        if (circularArray->front == -1)
            circularArray->front = circularArray->rear = 0;
        else if (circularArray->rear == SIZE_OF_TXBUFFER - 1)
            circularArray->rear = 0;
        else
            circularArray->rear++;

        circularArray->array[circularArray->rear] = data;
    }
}

/*
void TXStringEnqueue(char* str){
    int i = 0;
    int len = strlen(str);
    while(i < len){
        TXenqueue(&TXc_buffer, str[i]);
        i++;
}*/

char TXdequeue(TXCircularArray *circularArray) { // Get and remove a data from the circular array
    if (!TXisEmpty(circularArray)) {
        char data = circularArray->array[circularArray->front];
            
        if (circularArray->front == circularArray->rear)
            circularArray->front = circularArray->rear = -1;
        else if (circularArray->front == SIZE_OF_TXBUFFER - 1)
            circularArray->front = 0;
        else
            circularArray->front++;
        
        return data;
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

