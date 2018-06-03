#include "common.h"

/**********************************************************************
 * ----------------------- GLOBAL VARIABLES ---------------------------
 **********************************************************************/

char systemState = _IDLE;    // state of the system; _WAITING or _OPERATING
char transmitBuffer[64];         // holds the bytes to be transmitted
char receiveBuffer[64];         // holds the bytes to be transmitted
unsigned char Go, received;
char transmitCount;             // index to the transmitBuffer array
char receiveCount;
char toSend;
char move;
char loc[3];
char komsu[4];
char orient[2];
char key[2];
char senseSended;

/**********************************************************************
 * ----------------------- GLOBAL FUNCTIONS ---------------------------
 **********************************************************************/
/* transmits data using serial communication */
void transmitData()
{
	if (transmitCount < toSend) {
		TXREG1 = transmitBuffer[transmitCount];
		transmitCount++;
	}
	else {                  // all the bytes have been sent
		TXSTA1bits.TXEN = 0;// disable transmitter, will be enabled again in 250 msecs
                transmitCount = 0;
/*
                if(senseSended){
                    SetEvent(TASK1_ID,TRANS_MITTED);
                    senseSended = 0;
                }
  */
        }
}

/* Invoked when receive interrupt occurs; meaning that data is received */
void dataReceived()
{
	unsigned char rxbyte = RCREG1;

        if (rxbyte == '$') {   // stop character
                receiveCount=0;
        }
        else if (rxbyte == ':') {  // change task state
            received = 1;
         }
        receiveBuffer[receiveCount] = rxbyte;
        receiveCount++;
}


/* End of File : common.c */
