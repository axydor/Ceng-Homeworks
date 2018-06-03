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
        count++;  // get over the comma ','
        orient[0] = receiveBuffer[count++];  //
        orient[1] = receiveBuffer[count++];
        if(orient[1] != ',')
            count++;
        komsu[0]  = receiveBuffer[count++];
        komsu[1]  = receiveBuffer[count++];
        komsu[2]  = receiveBuffer[count++];
        komsu[3]  = receiveBuffer[count++];
        if(gotKey){ // So we got the key.
            key[0] = 'N';
            key[1] = 'E'; // Go to the DOOR
        }
        else{
        	key[0] = receiveBuffer[count++];
        	key[1] = receiveBuffer[count++];
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
// It tries to go to East If not ıt changes ıt states and move along y-axis
void go_to_east()
{
	if(komsu[EAST] == '0' || komsu[EAST] == '2')
		moveForward();
	else
	{
		turn_right();
		state = 'E'; // MOVE IN THE Y AXIS UNTIL IT FINDS A TURN TO EAST
	}
}

void go_to_west()
{
	if(komsu[WEST] == '0' || komsu[WEST] == '2')
		moveForward();
	else
	{
		turn_left();
		state = 'W'; // MOVE IN THE Y AXIS UNTIL IT FINDS A TURN TO EAST
	}
}
// It rotates until it is looking to the east
// and then goes to the west if possible
void just_east()
{
	if(orient[0] != '3' && orient[1] != '0' )
    {
    	if(orient[0] == '0') // If it is upward
    	   	turn_right();
    	else
    		turn_left();
    }
    else // Rotated so go to the east
    	go_to_east();
}
// It rotates until it is looking to the west
// and then goes to the east if possible
void just_west()
{
	if(orient[0] != '1' && orient[1] != '0' )
	{
		if(orient[0] == '0') // It is upward
			turn_left();
		else
			turn_right();
	}
	else
		go_to_west();
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
            {// JUST TRY TO MOVE ALONG THE EAST
            	just_east();
            }
            else if(key[1] == 'W')
            {// JUST TRY TO MOVE ALONG THE EAST
            	just_west();
            }
            else if(key[1] == ',')
            {
            	if(orient[0] != '0')
            		turn_left();
            	else
            	{
                    if(komsu[NORTH] == '0' || komsu[NORTH] == '2' )
                        moveForward();
                    else
                    {
                    	turn_right();
                    	state = 'S';
                    }            		
            	}
            }
        }
        // SOUTH
        else if(key[0] == 'S')
        {
            LATEbits.LATE0 = 1;
            if(key[1] == 'E')
            {// JUST TRY TO MOVE ALONG THE EAST
            	just_east();
            }
            else if(key[1] == 'W')
            {
            	just_west();
            }
            else if(key[1] == ',')
            {
                if(orient[0] != '2' && orient[1] != '0' ) // Rotate 
                    turn_right();
                else
                {
                    if(komsu[SOUTH] == '0' || komsu[SOUTH] == '2' )
                        moveForward();
                    else
                    {
                    	turn_right();
                    	state = 'S';
                    }
                }
            }
        }
        else if(key[0] == 'E')
        {
        	just_east();
        }
        else if(key[0] == 'W')
        {
            LATEbits.LATE0 = 1;
        }
    }
    // IT MOVES ALONG UP OR DOWN UNTIL IT FINDS A TURN TO THE EAST
    if(state == 'E')
    {
        if(komsu[EAST] == '0' || komsu[EAST] == '2' )
        {
        	if(orient[0] == '0')// It is moving upward
            	turn_right();
            else
            	turn_left();
            // So we have found the turn we can go to east
            state = '0';
        }
        else
        {  	// IF IT HITS THE BORDERS
        	if( (locy == '0' && orient[0] != '2') || (locy == '4' && orient[0] != '0') ) //
        		turn_right();
        	else
            	moveForward();
        }
    }
    // IT MOVES ALONG UP OR DOWN UNTIL IT FINDS A TURN TO THE WEST
    if(state == 'W')
    {
        if(komsu[WEST] == '0' || komsu[WEST] == '2' )
        {
        	if(orient[0] == '0')// It is moving upward
            	turn_left();
            else
            	turn_right();
            // So we have found the turn we can go to east
            state = '0';
        }
        else
        {  	// IF IT HITS THE BORDERS
        	if( (locy == '0' && orient[0] != '3') || (locy == '4' && orient[0] != [0]))
        		turn_right();
        	else
            	moveForward();
        }
    }
    // IT MOVES ALONG X-AXIS UNTIL IT FINDS A TURN TO THE SOUTH
    if(state == 'S')
    {
        if(komsu[SOUTH] == '0' || komsu[SOUTH] == '2' )
        {
        	if(orient[0] == '3')// It is moving east
            	turn_right();
            else
            	turn_left();
            // So we have found the turn we can go to east
            state = '0';
        }    
        else
        {
        	if( (locx[0] == '0' && orient[0] != '3') || ( (locx[0] == '1' && locx[1] == '5') ))
        			turn_right();
        	else
        		moveForward();
        }
    }
    // IT MOVES ALONG X-AXIS UNTIL IT FINDS A TURN TO THE NORTH
    else if(state == 'N')
    {
        if(komsu[NORTH] == '0' || komsu[NORTH] == '2' )
        {
        	if(orient[0] == '3')// It is moving east
            	turn_left();
            else
            	turn_right();
            // So we have found the turn we can go to east
            state = '0';
        }    
        else
        {
        	if( (locx[0] == '0' && orient[0] != '3') || ( (locx[0] == '1' && locx[1] == '5') ))
        			turn_right();
        	else
        		moveForward();
        }    	
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