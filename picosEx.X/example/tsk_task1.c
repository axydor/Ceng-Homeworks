#include "common.h"

/**********************************************************************
 * ----------------------- GLOBAL VARIABLES ---------------------------
 **********************************************************************/

extern char systemState;    // state of the system; _WAITING or _OPERATING
extern char Go, toSend, move;
extern char transmitBuffer[64];
extern char senseSended;
/**********************************************************************
 * ----------------------- LOCAL FUNCTIONS ----------------------------
 **********************************************************************/
/* A silly function for TASK1 */
void Sense()
{
    transmitBuffer[0] = '$';
    transmitBuffer[1] = 'S';
    transmitBuffer[2] = ':';
    toSend = 3;
}

/**********************************************************************
 * ------------------------------ TASK1 -------------------------------
 *
 * Prepares the data to be transmitted
 *
 **********************************************************************/
TASK(TASK1) 
{
	unsigned char count = 0;
        //WaitEvent(START_EVENT);
        //ClearEvent(START_EVENT);
	SetRelAlarm(ALARM_TSK1, 50, 50);
        while(1) {
                PIE1bits.RC1IE = 1;
                WaitEvent(ALARM_EVENT);
                ClearEvent(ALARM_EVENT);
                if(systemState == _RUNNING)
                {
                    if(GetResource(0) == E_OK)
                    {
                        Sense();
                        TXSTA1bits.TXEN = 1;// disable transmitter, will be enabled again in 250 msecs
                        //WaitEvent(TRANS_MITTED);
                        //ClearEvent(TRANS_MITTED);
                        LATEbits.LATE4 = ~LATEbits.LATE4;
                        ReleaseResource(0);

                    }
                }
        }
	TerminateTask();
}

/* End of File : tsk_task1.c */