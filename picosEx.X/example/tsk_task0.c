#include "common.h"

/**********************************************************************
 * ----------------------- GLOBAL VARIABLES ---------------------------
 **********************************************************************/
extern char receiveBuffer[64];
extern char systemState;
extern char transmitBuffer[64];
extern char received;
extern char toSend;
extern char Go;
extern char transmitCount;
extern char move;
extern char loc[3];
extern char komsu[4];
extern char orient[2];
extern char key[2];

/**********************************************************************
 * ----------------------- LOCAL FUNCTIONS ----------------------------
 **********************************************************************/
/* A silly function for TASK0 */
void foo0()
{
        //PIE1bits.RC1IE = 1;
	unsigned int n = 30;
	char i;
	for (i = 1; i >= 0; i--) {
		// transmit buffer is used to hold the resulting string
		// because this value will be transmitted to the PC
		transmitBuffer[i] = (n % 10) + '0';
		n /= 10;
	}
}
void moveForward()
{
    transmitBuffer[0] = '$';
    transmitBuffer[1] = 'F';
    transmitBuffer[2] = ':';
    toSend = 3;
    TXSTA1bits.TXEN = 1;// disable transmitter, will be enabled again in 250 msecs
}

void foo1()
{
	unsigned int n = 60;
	char i;
	for (i = 1; i >= 0; i--) {
		transmitBuffer[i] = (n % 10) + '0';
		n /= 10;
	}
}

void clearBuffer(char c)
{
    char i;
    for(i = c + '0'; i > 0 ; i--)
    {
        transmitBuffer[i] = '\0';
    }
}

void End()
{
    transmitBuffer[0] = '$';
    transmitBuffer[1] = 'E';
    transmitBuffer[2] = 'N';
    transmitBuffer[3] = 'D';
    transmitBuffer[4] = ':';
}

void parse_command()
{
    if(receiveBuffer[1] == 'G')
    {
        systemState = _RUNNING;
        SetEvent(TASK1_ID, START_EVENT);
    }
    if(receiveBuffer[1] == 'D')
    {
        LATEbits.LATE5 = 1;
        loc[0] = receiveBuffer[2];  // x coordinate
        loc[1] = receiveBuffer[3];  // x coordianate
        loc[2] = receiveBuffer[4];  // y coordinate
        orient[0] = receiveBuffer[6];  //
        orient[1] = receiveBuffer[7];
        komsu[0]  = receiveBuffer[8];
        komsu[1]  = receiveBuffer[9];
        komsu[2]  = receiveBuffer[10];
        komsu[3]  = receiveBuffer[11];
        key[0] = receiveBuffer[12];
        key[1] = receiveBuffer[13];
    }
    if(receiveBuffer[1] == 'A')
    {
        LATEbits.LATE6 = 1;
    }
}
void turn_left()
{
    transmitBuffer[0] = '$';
    transmitBuffer[1] = 'L';
    transmitBuffer[2] = ':';
    toSend = 3;
    TXSTA1bits.TXEN = 1;// disable transmitter, will be enabled again in 250 msecs
}
void move_it()
{
    if(key[0] == 'K')
    {
        LATEbits.LATE7 = 1;
    }
    else if(key[0] == 'N')
    {
        if(key[1] == 'E')
        {
            LATEbits.LATE2 = 1;
        }
        else if(key[1] == 'W')
        {
            LATEbits.LATE2 = 1;
        }
        else if(key[1] == ',')
        {
            LATEbits.LATE2 = 1;
        }
    }
    else if(key[0] == 'S')
    {
        if(key[1] == 'E')
        {
            LATEbits.LATE3 = 1;
        }
        else if(key[1] == 'W')
        {
            LATEbits.LATE3 = 1;
        }
        else if(key[1] == ',')
        {
            LATEbits.LATE3 = 1;
        }
    }
    else if(key[0] == 'E')
    {
        LATEbits.LATE1 = 1;
    }
    else if(key[0] == 'W')
    {
        LATEbits.LATE0 = 1;
    }


}

/**********************************************************************
 * ------------------------------ TASK0 -------------------------------
 * 
 * Prepares the data to be transmitted
 * 
 **********************************************************************/
TASK(TASK0) 
{
        unsigned char count = 0;
        //SetRelAlarm(ALARM_TSK0, 50, 50);
	while(1) {
            PIE1bits.RC1IE = 1;
            if(received){
                parse_command();
                received = 0;
                move = 1;
            }
            if(transmitCount==0)
            {
                if(systemState == _RUNNING)
                {
                    if(move){
                        if(GetResource(0) == E_OK){
                            for(count = 0; count < 10; count ++)
                                turn_left();
                            LATDbits.LATD0 = 1;
                            move = 0;
                            ReleaseResource(0);
                        }
                    }

                }
            }
	}
	TerminateTask();
}


/* End of File : tsk_task0.c */