#include <poll.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/socket.h>
#include <string.h>
#define PIPE(fd) socketpair(AF_UNIX, SOCK_STREAM, PF_UNIX, fd)

typedef struct coordinate
{
    int x;
    int y;    
} coordinate;

typedef struct object 
{
	char type;
	int health;
    coordinate cor;
    int pid;
    int fd;
} object;

typedef struct server_message
{
    coordinate pos;
    coordinate adv_pos;
    int object_count;
    coordinate object_pos[4];
} server_message;

typedef struct ph_message 
{
    coordinate move_request;
} ph_message;

int width, height, no_obstacles, no_hunters, no_preys;

void print_grid(int width, int height,object grid[height][width])
{
    int i,j,y;
    printf("+");
    for(y=0; y < width; y++)
        printf("-");
    
    printf("+\n");
    for(i=0; i < height; i++)
    {
        printf("|");
        for(j=0; j < width; j++)
        {
            printf("%c", grid[i][j].type);    
        }
        printf("|\n");
    }
    printf("+");
    for(y=0; y < width; y++)
        printf("-");
    printf("+\n");
    fflush(stdout);
}

void take_input(object grid[height][width])
{
	int i,j;
    for(i=0; i < height; i++)
    {
        for(j=0; j< width; j++)
        {
            grid[i][j].type = ' ';    
        }
    }
    scanf("%d", &no_obstacles);
    for(i=0; i < no_obstacles; i++)
    {
        int x,y;
        scanf("%d %d",&x, &y);
        grid[x][y].type = 'X';   
    }
    scanf("%d", &no_hunters);
    for(i=0; i < no_hunters; i++)
    {
        int x,y,health;
        scanf("%d %d %d", &x, &y, &health);
        grid[x][y].type = 'H';
        grid[x][y].health = health;
        grid[x][y].cor.x = x;
        grid[x][y].cor.y = y;
     }
    scanf("%d", &no_preys);
    for(i=0; i < no_preys; i++)
    {
        int x,y,health;
        scanf("%d %d %d", &x, &y, &health);
        grid[x][y].type = 'P';
        grid[x][y].health = health;
        grid[x][y].cor.x = x;
        grid[x][y].cor.y = y;
    }
}

coordinate MH(object grid[height][width],coordinate cor)
{
	int x = cor.x;
	int y = cor.y;
	char type = grid[cor.x][cor.y].type;
	char adv;
	coordinate adv_pos;
	if(type == 'H')
	{
		adv = 'P';
	}
	else
	{
		adv = 'H';
	}
	int MH = height + width ;
	int i,j,min_x,min_y;
	for(i=0; i < height; i++)
	{
		for(j=0; j < width; j++)
		{
			if(grid[i][j].type == adv)
			{
				int dist = abs( x - i ) + abs( y - j ) ;
				if( MH > dist )
				{
					MH = dist;
					min_x = i;
					min_y = j;
				}
			}
		}
	}
	adv_pos.x = min_x;
	adv_pos.y = min_y;
	return adv_pos;
}

int no_neighbours(object grid[height][width],coordinate cor, coordinate object_pos[4] )
{
	int x = cor.x;
	int y = cor.y;
	char type = grid[x][y].type;
	int i=0;
	if( 0 <= x - 1 )
	{
		if(grid[x-1][y].type == 'X' || grid[x-1][y].type == type )  // DOWN
		{
			object_pos[i].x = x-1;
			object_pos[i].y = y;
			i++;
		}
	}
	else if( x+1 < height)  //  UP
	{
		if(grid[x+1][y].type == 'X' || grid[x+1][y].type == type )
		{
			object_pos[i].x = x-1;
			object_pos[i].y = y;
			i++;
		}		
	}
	else if(0 <= y-1 )   // LEFT
	{
		if(grid[x][y-1].type == 'X' || grid[x][y-1].type == type)
		{
			object_pos[i].x = x;
			object_pos[i].y = y-1;
			i++;
		}
	}
	else if( y+1 < width )  // RIGHT
	{
		if(grid[x][y+1].type == 'X' || grid[x][y+1].type == type)
		{
			object_pos[i].x = x;
			object_pos[i].y = y+1;
			i++;
		}
	}
	return i;
}

int main()
{
	int i, j, z, lock=0;

    scanf("%d %d", &width, &height);
	object grid[height][width];
	take_input(grid);
    print_grid(width, height, grid);

    struct pollfd fds[ no_hunters + no_preys ];  //	ARRAY OF FILE DESCRIPTORS
    int ip=-1;            //  ARRAY OF PID, PID AND RELATED FD ARE AT THE SAME INDEX ON THE TWO ARRAYS(fds[],control[]).
    int child,k;
    
    // FORK TIME
	    for(i=0; i < height; i++)
	    {
		   	for(j=0; j < width; j++)
		   	{
		   		if(grid[i][j].type == 'H' || grid[i][j].type == 'P' )
		   		{
			   		ip++;
			        int fd[2];
			        PIPE(fd);
			        int h_chil;
			        if( grid[i][j].pid = fork() )
			        {
			 	    	printf("PARENT- %d\n",ip );
		 	        fflush(stdout);

			        	close(fd[1]);
			        	fds[ip].fd       = fd[0];
			 	        fds[ip].events   = POLLIN;
				        grid[i][j].fd    = fd[0];

			            server_message s_m;
			            memset(&s_m,0,sizeof(server_message));
			            coordinate object_pos[4];
			            s_m.adv_pos = MH(grid, grid[i][j].cor); // LOCATION OF CLOSES ADVERSARY
			            s_m.object_count =  no_neighbours(grid, grid[i][j].cor,s_m.object_pos);// NUMBER OF NEIGHBOURING OBJECT
			            if(write(fd[0],&s_m,sizeof(server_message)) == -1 )
			            	perror("SERVER-WRITE-ERROR\n");
			            char line[80];
			            read(fd[0],line,80);
			            printf("%s\n",line );
			            fflush(stdout);

			        }
			 	    else
			 	    {   
			 	    	printf("CHILD- %d\n",ip );
		 	        fflush(stdout);

			 	        dup2(fd[1],0);
			 	        dup2(fd[1],1);
   			            close(fd[0]);
			            close(fd[1]);

			 	        char s_height[3];
			 	        char s_width[3];
			 	        sprintf( s_height, "%d", height );
			 	        sprintf( s_width, "%d", width );
			            if(grid[i][j].type == 'H')
			            {
				            char* newargv[] = { "./hunt", s_height, s_width, NULL };
				            char* newenv[] = { NULL };
				            if( execve( "./hunt", newargv, newenv ) == -1)
				            	write(2,"EXECVE ERROR",20);		                	
			            }
			            else
			            {
			                char* newargv[] = { "./prey", s_height, s_width, NULL };
			                char* newenv[] = { NULL };
				            if( execve( "./prey", newargv, newenv ) == -1)
				            	write(2,"EXECVE ERROR",20);		  
			            }
			        }
			    }
			}
		}

/*
		for(z=0; z <no_hunters+no_preys; z++)
		{
			for(i=0; i < height; i++)
			{
				for(j=0; j < width; j++)
				{
					if(grid[i][j].type == 'H' || grid[i][j].type == 'P' )
					{
						if(fds[z].fd == grid[i][j].fd)
						{
							;
						}
					}
				}
			}
		}

*/
/*
	    while(1)
	    {
	    	int pid;
	        char line[128];
	        int retval;
	        retval = poll(fds,no_hunters+no_preys,-1);
	        if(retval == -1)
	        {
	        	perror("POLL");
	        }
	        for(i=0; i <no_hunters+no_preys; i++)  // CHECK THE ALIVE PROCESSES (WHICH ARE MORTAL :) 
	        {
	        	if(type_pid[i].pid == 0) // THAT PROCESS IS KILLED
	        	{
	        		continue;
	        	}
	        	if(fds[i].revents & POLLIN)
	        	{
	        		int x = message.move_request.x;
			     	int y = message.move_request.y
			        char being_type = type_pid[i].type;
			        char tile_type  = grid[x][y].type;
			        ph_message message;
	        		read(0,&message;,sizeof(ph_message));
	        		if( 0 <= x < height && 0 <= y < width) // CHECK IF THE REQUEST IS VALID
	        		{
	        			if(being_type == 'H' && tile_type == 'P')  // HUNTER KILSS PREY
	        			{
	        				kill()
	        			}
	        		}
	        		//send(fd[i])
	        	}
	        }

	        if(no_hunters == 0 || no_preys == 0)
	        {
	        	//massacare();
	        	break;
	        }
	    }  
*/
	sleep(1);
	for(i=0; i < height; i++)
	{
		for(j=0; j < width; j++)
		{
			if(grid[i][j].type == 'H' || grid[i][j].type == 'P' )
			{
				int dip = grid[i][j].pid;
    		    printf("pid:%d\n",dip);
    		    close(grid[i][j].fd);
    		    kill(dip,SIGTERM);
				waitpid(dip,NULL,0);
			}
		}
	}
    return 0;
    
}
