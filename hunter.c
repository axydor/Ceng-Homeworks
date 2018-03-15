#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <poll.h>

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
    int i,retval;
    int height,width;
    int MH; // Manhattan Distance
    coordinate cor;
    ph_message message;
    server_message s_m;
 
    struct pollfd fds[2];

    fds[0].fd = STDIN_FILENO;
    fds[0].events = POLLIN;

    height = atoi(args[1]);

    width  = atoi(args[2]);
    retval = poll(fds,1,-1);
    if( retval < 0 )
        write(2,"HUNTER-POLL-ERROR\n",80);
    
    read(0,&s_m,sizeof(server_message));

    cor = s_m.pos;

    while(1)
    {
        int N=1,S=1,L=1,R=1;        

        retval = poll(fds,2,-1);
        read(0,&s_m,sizeof(server_message));
    
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
            int posib_x = s_m.object_pos[i].x;
            int posib_y = s_m.object_pos[i].y;
            if(N && 0 <= cor.x-1 ) // UP TILE IS NOT OBSTRUCTED
            {
                new_MH = abs(cor.x-1 - posib_x) + abs(cor.y - posib_y);
                if( new_MH < MH)
                {
                    MH = new_MH;
                    message.move_request.x = cor.x-1;
                    message.move_request.y = cor.y;
                    write(1,&message,sizeof(message));
                    usleep(10000*(1+rand()*100/9));
                    break;
                }
            }
            else if(S && cor.x+1 < height)  // DOWN TILE IS NOT OBSTRUCTED
            {
                new_MH = abs(cor.x+1 - posib_x) + abs(cor.y - posib_y);
                if( new_MH < MH)
                {
                    MH = new_MH;
                    message.move_request.x = cor.x+1;
                    message.move_request.y = cor.y;
                    write(1,&message,sizeof(message));
                    usleep(10000*(1+rand()*100/9));                    
                    break;
                }                
            }
            else if(L && 0 <= cor.y-1)   // LEFT TILE IS NOT OBSTURCTED
            {
                new_MH = abs(cor.x - posib_x) + abs(cor.y-1 - posib_y);
                if( new_MH < MH)
                {
                    MH = new_MH;
                    message.move_request.x = cor.x;
                    message.move_request.y = cor.y-1;
                    write(1,&message,sizeof(message));
                    usleep(10000*(1+rand()*100/9));
                    break;
                }                   
            }
            else if(R && cor.y+1 < width) // RIGHT TILE IS NOT OBSTRUCTED
            {
                new_MH = abs(cor.x - posib_x) + abs(cor.y+1 - posib_y);
                if( new_MH < MH)
                {
                    MH = new_MH;
                    message.move_request.x = cor.x;
                    message.move_request.y = cor.y+1;
                    write(1,&message,sizeof(message));
                    usleep(10000*(1+rand()*100/9));
                    break;
                }                   
            }
            else // NO POSSIBLE MOVE;
            {
                message.move_request.x = cor.x;
                message.move_request.y = cor.y;
                write(1,&message,sizeof(message));
                usleep(10000*(1+rand()*100/9));
                break;                
            }
        }
    }    

    return 0;    
}
