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
#ifndef COMMUNICATION_PROTOCOLS_H
#define	COMMUNICATION_PROTOCOLS_H

#include <xc.h> // include processor files - each processor file is guarded.  

#define STATE_DOLLAR (1) // we discard everything until a dollar is found
#define STATE_TYPE (2) // we are reading the type of msg until a comma is found
#define STATE_PAYLOAD (3) // we read the payload until an asterix is found
#define NEW_MESSAGE (1) // new message received and parsed completely
#define NO_MESSAGE (0) // no new messages
#define TYPE_LEN 5 // 'PCTH' + '\0'
#define PAYLOAD_LEN 7 // p2bytes for 1int + comma + 2bytes for 1int + '*' + '\0'
#define MSG_INFO "PCTH"

typedef struct {
int state;
char msg_type[TYPE_LEN]; 
char msg_payload[PAYLOAD_LEN]; 
int index_type;
int index_payload;
} parser_state;

parser_state pstate;

void protocol_setup(){
    pstate.state = STATE_DOLLAR;
    pstate.index_type = 0;
    pstate.index_payload = 0;
}

int parse_byte(parser_state* ps, char byte){
    switch (ps->state){
        case STATE_DOLLAR:
            if (byte == '$'){
                ps->state = STATE_TYPE;
                ps->index_type = 0;
            }
        break;
        case STATE_TYPE:
            if (byte == ','){
                ps->state = STATE_PAYLOAD;
                ps->msg_type[ps->index_type] = '\0';
                ps->index_payload = 0;
            } else if (ps->index_type == TYPE_LEN) {
                ps->state = STATE_DOLLAR;
                ps->index_type = 0;
                return -1;
            } else {
                ps->msg_type[ps->index_type] = byte;
                ps->index_type++;
            }
        break;
        case STATE_PAYLOAD:
            if (byte == '*') {
                ps->state = STATE_DOLLAR;
                ps->msg_payload[ps->index_payload] = '\0';
                return NEW_MESSAGE;
            } else if (ps->index_payload == PAYLOAD_LEN){
                ps->state = STATE_DOLLAR;
                ps->index_payload = 0;
                return -1;
            } else {
                ps->msg_payload[ps->index_payload] = byte;
                ps->index_payload++;
            }
        break;
    }
    return NO_MESSAGE;
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

