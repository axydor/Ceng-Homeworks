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
extern char locx[2];
extern char locy;
extern char komsu[4]; // a, b, c, d // up , right, down, left
extern char orient[2];
extern char key[2];
extern char state;
extern char direction;
extern char gotKey;

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
    unsigned char count = 2;
    if(receiveBuffer[1] == 'G')
    {
        systemState = _RUNNING;
        SetEvent(TASK1_ID, START_EVENT);
    }
    if(receiveBuffer[1] == 'D')
    {
        locx[0] = receiveBuffer[count++];  // x coordinate
        locx[1] = receiveBuffer[count++];  // x coordianate
        if(locx[1] != ',')
            count++;
        locy = receiveBuffer[count++];
        count++;
        orient[0] = receiveBuffer[count++];  //
        orient[1] = receiveBuffer[count++];
        if(orient[1] != ',')
            count++;
        komsu[0]  = receiveBuffer[count++];
        komsu[1]  = receiveBuffer[count++];
        komsu[2]  = receiveBuffer[count++];
        komsu[3]  = receiveBuffer[count++];
        if(!gotKey){
            key[0] = 'N';
            key[1] = 'E'; // Go to the DOOR
        }
    }
    if(receiveBuffer[1] == 'A')
    {
        LATEbits.LATE6 = 1;
    }
}
void turn_left()
{
    unsigned char count;
    transmitBuffer[0] = '$';
    transmitBuffer[1] = 'L';
    transmitBuffer[2] = ':';
    toSend = 3;
    for(count = 0; count < 10; count ++)
        TXSTA1bits.TXEN = 1;// disable transmitter, will be enabled again in 250 msecs
}
void turn_right()
{
    unsigned char count;
    transmitBuffer[0] = '$';
    transmitBuffer[1] = 'R';
    transmitBuffer[2] = ':';
    toSend = 3;
    for(count = 0; count < 10; count ++)
        TXSTA1bits.TXEN = 1;// disable transmitter, will be enabled again in 250 msecs
}
void pick_key()
{
    unsigned char count;
    transmitBuffer[0] = '$';
    transmitBuffer[1] = 'R';
    transmitBuffer[2] = ':';
    toSend = 3;
    TXSTA1bits.TXEN = 1;// disable transmitter, will be enabled again in 250 msecs
}
void move_it()
{
    if(state == '0')
    {
        if(key[0] == 'K')
        {
            LATEbits.LATE7 = 1;
            gotKey = 1;
            pick_key();
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
            LATEbits.LATE0 = 1;
            if(key[1] == 'E')
            {// MOVE ALONG THE EAST
                if(orient[0] != '3' && orient[1] != '0' )
                    turn_left();
                else{
                    if(komsu[EAST] == '0' || komsu[EAST] == '2')
                    {
                        moveForward();
                    }
                    else
                    {
                        turn_right();
                        state = 'E'; // MOVE UNTIL IT TURNS TO EAST
                    }
                }
            }
            else if(key[1] == 'W')
            {
                if(orient[0] != '1' && orient[1] != '0' )
                    turn_left();
                else
                {
                    if(komsu[WEST] == '0' || komsu[WEST] == '2')
                    {
                        moveForward();
                    }
                }
            }
            else if(key[1] == ',')
            {
                if(orient[0] != '2' && orient[1] != '0' )
                    turn_right();
                else
                {
                    if(komsu[SOUTH] == '0' || komsu[SOUTH] == '2' )
                    {
                        moveForward();
                    }
                }
            }
        }
        else if(key[0] == 'E')
        {
            LATEbits.LATE1 = 1;
            if(orient[0] == '3' && orient[1] == '0')
            {
                moveForward();

            }
            else
                turn_left();
        }
        else if(key[0] == 'W')
        {
            LATEbits.LATE0 = 1;
        }
    }
    if(state == 'E')
    {
        if(komsu[EAST] == '0' || komsu[EAST] == '2' )
            turn_left();
        else
            moveForward();
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
                            move_it();
                            //LATDbits.LATD0 = 1;
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