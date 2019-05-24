#include "common.h"

/**********************************************************************
 * ----------------------- GLOBAL VARIABLES ---------------------------
 **********************************************************************/

char systemState = _IDLE;    // state of the system; _WAITING or _OPERATING
char transmitBuffer[64];         // holds the bytes to be transmitted
char receiveBuffer[64];         // holds the bytes to be transmitted
unsigned char Go, received;
char transmitCount;// index to the transmitBuffer array
char receiveCount; // Length of the string to be receive over receiveBuf
char toSend;       // Length of the string to be send over transmitBuffer
char move;         // flag whether we make a movement according to coming respo
/// Below value got from the Sense Respond send by the simulation
char locx[2];      // x-axis of robot
char locy;         // location y-axis of robot
char komsu[4];     // neighbour information
char orient[2];    // Orienteiton of the robot
char key[2];       // direction of the key wrt to robot
///////////////////////////////////
char senseSended;  // flag that to check sense respond is send to simulation
char state = '0';
char direction = '0';
char gotKey; // FLAG whether we got the key
char gotKey = 0;
char no_of_alarms = 0;


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
