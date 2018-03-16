#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <poll.h>
#include <string.h>

typedef struct coordinate
{
    int x;
    int y;    
} coordinate;

typedef struct ph_message 
{
    coordinate move_request;
} ph_message;


typedef struct server_message
{
    coordinate pos;
    coordinate adv_pos;
    int object_count;
    coordinate object_pos[4];
} server_message;

int main(int argc, char* args[])
{
    int i;
    int height = atoi(args[1]);
    int width = atoi(args[2]);
    int MH; // Manhattan Distance
    coordinate cor;
    height = atoi(args[1]);
    width  = atoi(args[2]);
    while(1)
    {

        ph_message message;
        
        
        server_message s_m;
        
        
        if(read(0,&s_m,sizeof(server_message)))
        {
            int N=1,S=1,L=1,R=1;        
            //fprintf(stderr, "HUNTER-READ -MESSAGE\n");
            cor = s_m.pos;
            MH = abs(cor.x - s_m.adv_pos.x) + abs(cor.y - s_m.adv_pos.y);

            for(i=0; i < s_m.object_count; i++)  //  CHECK WHICH TILE MOVEABLE;  
            {
                if(cor.x-1 == s_m.object_pos[i].x)
                {
                    N = 0;
                }
                else if(cor.x+1 == s_m.object_pos[i].x)
                {
                    S = 0;
                }    
                if(cor.y-1 == s_m.object_pos[i].y)
                {
                    L = 0;
                }
                if(cor.y-1 == s_m.object_pos[i].y)
                {
                    R = 0;    
                }
            }

            for(i=0; i <  4 - s_m.object_count; i++)   // CREATE AND SEND A REQUEST TOs MOVE
            {
                int new_MH;
                if(N && 0 <= cor.x-1 ) // UP TILE IS NOT OBSTRUCTED
                {
                    new_MH = abs(cor.x-1 - s_m.adv_pos.x) + abs(cor.y - s_m.adv_pos.y);
                    if( new_MH < MH)
                    {
                        MH = new_MH;
                        message.move_request.x = cor.x-1;
                        message.move_request.y = cor.y;
                        if(write(1,&message,sizeof(ph_message)) < 0)
                            perror("HUNTER OR PREY WRITE ERROR");
                        usleep(10000*(1+rand()%9));
                        break;
                    }
                }
                if(S && cor.x+1 < height)  // DOWN TILE IS NOT OBSTRUCTED
                {
                    new_MH = abs(cor.x+1 - s_m.adv_pos.x) + abs(cor.y - s_m.adv_pos.y);
                    if( new_MH < MH)
                    {
                        MH = new_MH;
                        message.move_request.x = cor.x+1;
                        message.move_request.y = cor.y;
                        if(write(1,&message,sizeof(ph_message)) < 0)
                            perror("HUNTER OR PREY WRITE ERROR");
                        usleep(10000*(1+rand()%9));                    
                        break;
                    }                
                }
                if(L && 0 <= cor.y-1)   // LEFT TILE IS NOT OBSTURCTED
                {
                    new_MH = abs(cor.x - s_m.adv_pos.x) + abs(cor.y-1 - s_m.adv_pos.y);
                    if( new_MH < MH)
                    {
                        MH = new_MH;
                        message.move_request.x = cor.x;
                        message.move_request.y = cor.y-1;
                        if(write(1,&message,sizeof(ph_message)) < 0)
                            perror("HUNTER OR PREY WRITE ERROR");
                        usleep(10000*(1+rand()%9));
                        break;
                    }                   
                }
                if(R && cor.y+1 < width) // RIGHT TILE IS NOT OBSTRUCTED
                {
                    new_MH = abs(cor.x - s_m.adv_pos.x) + abs(cor.y+1 - s_m.adv_pos.y);
                    if( new_MH < MH)
                    {
                        MH = new_MH;
                        message.move_request.x = cor.x;
                        message.move_request.y = cor.y+1;
                        if(write(1,&message,sizeof(ph_message)) < 0)
                            perror("HUNTER OR PREY WRITE ERROR");
                        usleep(10000*(1+rand()%9));
                        break;
                    }                   
                }
                else // NO POSSIBLE MOVE;
                {
                    message.move_request.x = cor.x;
                    message.move_request.y = cor.y;
                    if(write(1,&message,sizeof(ph_message)) < 0)
                        perror("HUNTER OR PREY WRITE ERROR");
                    usleep(10000*(1+rand()%9));
                    break;                
                }
            }
            //fprintf(stderr, "HUNTER-WROTE\n");
            
        }
    }    

    return 0;    
}
