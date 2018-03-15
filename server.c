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
    //coordinate cor;
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

void print_grid(object grid[height][width], int height, int width)
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
        //grid[x][y].cor.x = x;
        //grid[x][y].cor.y = y;
     }
    scanf("%d", &no_preys);
    for(i=0; i < no_preys; i++)
    {
        int x,y,health;
        scanf("%d %d %d", &x, &y, &health);
        grid[x][y].type = 'P';
        grid[x][y].health = health;
        //grid[x][y].cor.x = x;
        //grid[x][y].cor.y = y;
    }
}

coordinate MH(object grid[height][width],int a,int b)
{
	int x = a;
	int y = b;
	char type = grid[x][y].type;
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

int no_neighbours(object grid[height][width],int a,int b, coordinate object_pos[4] )
{
	int x = a;
	int y = b;
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

void hunter_kills_prey(object grid[height][width],int i,int j,int x,int y)
{
	int pid_prey = grid[x][y].pid;
    close(grid[x][y].fd);
    kill(pid_prey,SIGTERM);
    waitpid(pid_prey,NULL,0);

    grid[x][y].type = 'H';
    grid[x][y].fd = grid[i][j].fd;
    grid[x][y].pid = grid[i][j].pid;
    grid[x][y].health += grid[i][j].health - 1;
    
    grid[i][j].type = ' ';
    grid[i][j].fd = 0;
    grid[i][j].pid = 0;
    grid[i][j].health = 0;
    server_message s_m;
    memset(&s_m,0,sizeof(server_message));
    s_m.pos.x = x;
    s_m.pos.y = y;
    coordinate object_pos[4];
    s_m.adv_pos = MH(grid, x, y); // LOCATION OF CLOSES ADVERSARY
    s_m.object_count =  no_neighbours(grid,x,y,s_m.object_pos);// NUMBER OF NEIGHBOURING OBJECT
    if(write(grid[x][y].fd,&s_m,sizeof(server_message)) == -1 )
        perror("HUNTER-KILLS-PREY\n");
    
    no_preys--;
}

void cant_move(object grid[height][width],int i,int j)
{
    server_message s_m;
    memset(&s_m,0,sizeof(server_message));
    s_m.pos.x = i;
    s_m.pos.y = j;
    coordinate object_pos[4];
    s_m.adv_pos = MH(grid, i, j); // LOCATION OF CLOSES ADVERSARY
    s_m.object_count =  no_neighbours(grid,i,j,s_m.object_pos);// NUMBER OF NEIGHBOURING OBJECT
    if(write(grid[i][j].fd,&s_m,sizeof(server_message)) == -1 )
        perror("CANT-MOVE\n");
}

void hunter_moves(object grid[height][width],int i,int j,int x,int y)
{
	int new_health = grid[i][j].health - 1;

	//	CLEAN THE PREVIOUS TILE

	if(new_health <= 0)
	{
		grid[i][j].type = ' ';
		grid[i][j].fd=0;
		grid[i][j].pid=0;
		grid[i][j].health=0;	
		int pid_hunter = grid[i][j].pid;
		close(grid[i][j].fd);
		kill(pid_hunter,SIGTERM);
		waitpid(pid_hunter,NULL,0);

	}
	else
	{
	    grid[x][y].health = grid[i][j].health - 1;
	    grid[x][y].type = 'H';
	    grid[x][y].fd = grid[i][j].fd;
	    grid[x][y].pid = grid[i][j].pid;

		grid[i][j].type = ' ';
		grid[i][j].fd=0;
		grid[i][j].pid=0;
		grid[i][j].health=0;	

	    server_message s_m;
	    memset(&s_m,0,sizeof(server_message));
	    s_m.pos.x = x;
	    s_m.pos.y = y;
	    coordinate object_pos[4];
	    s_m.adv_pos = MH(grid, x, y); // LOCATION OF CLOSES ADVERSARY
	    s_m.object_count =  no_neighbours(grid,x,y,s_m.object_pos);// NUMBER OF NEIGHBOURING OBJECT
	    if(write(grid[x][y].fd,&s_m,sizeof(server_message)) == -1 )
	        perror("HUNTER-MOVES\n");
	}
		
}
void prey_moves(object grid[height][width],int i,int j,int x,int y)
{
    grid[x][y].type = 'P';
    grid[x][y].fd = grid[i][j].fd;
    grid[x][y].pid = grid[i][j].pid;
    grid[x][y].health = grid[i][j].health;

    
    grid[i][j].type = ' ';
    grid[i][j].fd = 0;
    grid[i][j].pid = 0;
    grid[i][j].health = 0;	

    server_message s_m;
    memset(&s_m,0,sizeof(server_message));
    s_m.pos.x = x;
    s_m.pos.y = y;
    coordinate object_pos[4];
    s_m.adv_pos = MH(grid, x, y); // LOCATION OF CLOSES ADVERSARY
    s_m.object_count =  no_neighbours(grid,x,y,s_m.object_pos);// NUMBER OF NEIGHBOURING OBJECT
    if(write(grid[x][y].fd,&s_m,sizeof(server_message)) == -1 )
        perror("PREY-MOVES\n");
}

void massacre(object grid[height][width])
{
	printf("MASSACRE IS INEVITABLE\n");
	int i,j;
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
}

int main()
{
	int i, j, z, lock=0;

    scanf("%d %d", &width, &height);
	object grid[height][width];
	take_input(grid);
    print_grid(grid,height,width);

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

    sleep(1);
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
                    	printf("PRINT THIS TWO TIMES\n");
                    	fflush(stdout);
                        server_message s_m;
                        memset(&s_m,0,sizeof(server_message));
                        s_m.pos.x = i;
                        s_m.pos.y = j;
                        coordinate object_pos[4];
                        s_m.adv_pos = MH(grid, i,j); // LOCATION OF CLOSES ADVERSARY
                        s_m.object_count =  no_neighbours(grid,i,j,s_m.object_pos);// NUMBER OF NEIGHBOURING OBJECT
                        if(write(fds[z].fd,&s_m,sizeof(server_message)) == -1 )
                            perror("SERVER-WRITE-ERROR\n");
                    }
                }
            }
        }
    }
    while(1)
    {
    	int z;
    	printf("IN THE WHILE\n");
    	fflush(stdout);
        int map_updated=0;
        int pid;
        char line[128];
        int retval;
        retval = poll(fds,no_hunters+no_preys,-1);
        if(retval == -1)
        {
            perror("POLL");
        }
        for(z=0; z < no_hunters+no_preys; z++)  // CHECK THE ALIVE PROCESSES (WHICH ARE MORTAL :) 
        {
            if(fds[z].revents & POLLIN)
            {
            	printf("fd:%d\n", fds[z].fd);
            	fflush(stdout);
                ph_message message;
                if(read(fds[z].fd,&message,sizeof(ph_message)) == -1)
                	perror("READ-ERROR\n");
                for(i=0; i < height; i++)
                {
                    for(j=0; j < width; j++)
                    {
                        if(fds[z].fd == grid[i][j].fd)
                        {
                        	sleep(2);
                            int x = message.move_request.x;
                            int y = message.move_request.y;
                            char being_type = grid[i][j].type;
                            char tile_type  = grid[x][y].type;
                            printf("MOVE REQ x:%d -- y:%d\n",x,y );
                            if(being_type == 'H') // HUNTER REQUESTS
                            {
                                if(tile_type == 'P')  // HUNTER KILSS PREY
                                {
                                    hunter_kills_prey(grid,i,j,x,y);
                                    map_updated=1;
                                }
                                else if(tile_type == 'X')
                                {
                                    cant_move(grid,i,j);
                                }
                                else if(tile_type == 'H')
                                {
                                    cant_move(grid,i,j);
                                }
                                else if(tile_type == ' ')
                                {
                                    hunter_moves(grid,i,j,x,y);
                                    map_updated=1;
                                }				        		
                            }
                            else if(being_type == 'P') // PREY REQUESTS
                            {
                                if(tile_type == 'H') 
                                {
                                    cant_move(grid,i,j);
                                }
                                else if(tile_type == 'X')
                                {
                                    cant_move(grid,i,j);
                                }
                                else if(tile_type == 'P')
                                {
                                    cant_move(grid,i,j);
                                }
                                else if(tile_type == ' ')
                                {
                                    prey_moves(grid,i,j,x,y);
                                    map_updated=1;
                                }							        	
                            }
                        }
                    }
                }
            }
        }
        if(map_updated)
        {
            print_grid(grid,height,width);
        }
        if(no_hunters == 0 || no_preys == 0)
        {
            //massacare();
            break;
        }
    }  
	massacre(grid);
    return 0;
    
}
