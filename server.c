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

int width, height, no_obstacles, no_h, no_p;


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
    int fd;  // HOLDS FD SO THAT WE WILL CLOSE PREY'S PIPE WHEN HUNTER EATS IT
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
    scanf("%d", &no_h);
    for(i=0; i < no_h; i++)
    {
        int x,y,health;
        scanf("%d %d %d", &x, &y, &health);
        grid[x][y].type = 'H';
        grid[x][y].health = health;
        //grid[x][y].cor.x = x;
        //grid[x][y].cor.y = y;
     }
    scanf("%d", &no_p);
    for(i=0; i < no_p; i++)
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
	if( x+1 < height)  //  UP
	{
		if(grid[x+1][y].type == 'X' || grid[x+1][y].type == type )
		{
			object_pos[i].x = x+1;
			object_pos[i].y = y;
			i++;
		}		
	}
	if(0 <= y-1 )   // LEFT
	{
		if(grid[x][y-1].type == 'X' || grid[x][y-1].type == type)
		{
			object_pos[i].x = x;
			object_pos[i].y = y-1;
			i++;
		}
	}
	if( y+1 < width )  // RIGHT
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

void hunter_kills_prey(struct pollfd fds[no_h+no_p],coordinate cor_of_fd[no_h+no_p],object grid[height][width],int i,int j,int x,int y,int z)
{
	int a=0;
	int h_fd_i = z;
	int p_fd_i;
	for(a=0; a < no_h + no_p;a++)
	{
		if(fds[a].fd == grid[x][y].fd)
		{
			p_fd_i = a;
		}
	}
	int pid_prey = grid[x][y].pid;
    close(grid[x][y].fd);

    fds[p_fd_i].fd = 0;
    cor_of_fd[p_fd_i].x = -1;
    cor_of_fd[p_fd_i].y = -1;

    kill(pid_prey,SIGTERM);
    waitpid(pid_prey,NULL,0);

    grid[x][y].type = 'H';
    grid[x][y].fd = grid[i][j].fd;
    grid[x][y].pid = grid[i][j].pid;
    grid[x][y].health += grid[i][j].health - 1;
	cor_of_fd[h_fd_i].x = x;
	cor_of_fd[h_fd_i].y = y;
    
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
    
    no_p--;
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

void hunter_moves(struct pollfd fds[no_h+no_p],coordinate cor_of_fd[no_h+no_p],object grid[height][width],int i,int j,int x,int y,int z)
{
	int new_health = grid[i][j].health - 1;

	//	CLEAN THE PREVIOUS TILE


	if(new_health == 0)
	{
		int pid_hunter = grid[i][j].pid;
		grid[i][j].type = ' ';
		grid[i][j].fd=0;
		grid[i][j].pid=0;
		grid[i][j].health=0;	
		cor_of_fd[z].x = -1;
		cor_of_fd[z].y = -1;

		close(fds[z].fd);
		kill(pid_hunter,SIGTERM);
		waitpid(pid_hunter,NULL,0);
		printf("HUNTER DIED\n");
	}
	else
	{
	    grid[x][y].health = new_health;
	    grid[x][y].type = 'H';
	    grid[x][y].fd = grid[i][j].fd;
	    grid[x][y].pid = grid[i][j].pid;
	    cor_of_fd[z].x = x;
	    cor_of_fd[z].y = y;

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
	    printf("HUNTER MOVED FROM %d-%d to %d,%d\n",i,j,x,y );
	    s_m.object_count =  no_neighbours(grid,x,y,s_m.object_pos);// NUMBER OF NEIGHBOURING OBJECT
	    if(write(fds[z].fd,&s_m,sizeof(server_message)) < 0 )
	        perror("HUNTER-ERROR-MOVES\n");
	}
		
}
void prey_moves(coordinate cor_of_fd[no_h+no_p],object grid[height][width],int i,int j,int x,int y)
{
	int p_fd_i = grid[i][j].fd;

	cor_of_fd[p_fd_i].x = x;
	cor_of_fd[p_fd_i].y = y;

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

    struct pollfd fds[  no_p + no_h ];  //	ARRAY OF FILE DESCRIPTORS
    coordinate cor_of_fd[ no_h + no_p ];
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

                    cor_of_fd[ip].x = i;
                    cor_of_fd[ip].y = j;

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
    sleep(2);
    for(z=0; z <no_h+no_p; z++)
    {
    	int i = cor_of_fd[z].x;
    	int j = cor_of_fd[z].y;
        
        server_message s_m;
        memset(&s_m,0,sizeof(server_message));
        s_m.pos.x = i;
        s_m.pos.y = j;
        coordinate object_pos[4];
        s_m.adv_pos = MH(grid, i,j); // LOCATION OF CLOSES ADVERSARY
        s_m.object_count =  no_neighbours(grid,i,j,s_m.object_pos);// NUMBER OF NEIGHBOURING OBJECT
        if(write(fds[z].fd,&s_m,sizeof(server_message)) <= 0 )
            perror("SERVER-WRITE-ERROR\n");
    }
/*
        for(i=0; i < height; i++)
        {
            for(j=0; j < width; j++)
            {
                if(grid[i][j].type == 'H' || grid[i][j].type == 'P' )
                {
                    if(fds[z].fd == grid[i][j].fd)
                    {
                    	//printf("PRINT THIS TWO TIMES\n");
                    	//fflush(stdout);
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

    }*/
    printf("fd[0]:%d\n", fds[0].fd);
    printf("fd[1]:%d\n", fds[1].fd);
    printf("no_p:%d no_h:%d\n",no_p,no_h );
    while(1)
    {
    	int z;
    	fflush(stdout);
        int retval;
        retval = poll(fds,no_p+no_h,0);
        if(retval < 0)
        {
        	printf("retval is 0\n");
        	fflush(stdout);
        	continue;
        }
        for(z=0; z < no_p + no_h; z++)  // CHECK THE ALIVE PROCESSES (WHICH ARE MORTAL :) 
        {
	    // ALL FILE DESCRIPTORS OF ALIVE CHILDREN ARE READY
        	int map_updated=0;
            if(fds[z].revents & POLLIN)
            {
            	printf("fd:%d\n",fds[z].fd);
            	fflush(stdout);
                ph_message message;
                if(read(fds[z].fd,&message,sizeof(ph_message)) == -1)
                	perror("READ-ERROR\n");

                int i = cor_of_fd[z].x;
                int j = cor_of_fd[z].y;


                int x = message.move_request.x;
                int y = message.move_request.y;
                char being_type = grid[i][j].type;
                char tile_type  = grid[x][y].type;

                printf("MOVE REQ OF %c AT THE x:%d - y:%d to x:%d - y:%d\n",being_type,i,j,x,y );
                fflush(stdout);
                if(being_type == 'H') // HUNTER REQUESTS
                {
                    if(tile_type == 'P')  // HUNTER KILSS PREY
                    {
                        hunter_kills_prey(fds,cor_of_fd,grid,i,j,x,y,z);
                        map_updated=1;
                        printf("HUNTER-KILLS\n");
                    	
                    }
                    else if(tile_type == 'X')
                    {
                        cant_move(grid,i,j);
                        printf("HUNTER-CANT-MOVED\n");

                    }
                    else if(tile_type == 'H')
                    {
                        cant_move(grid,i,j);
                        printf("HUNTER-CANT-MOVED\n");

                    }
                    else if(tile_type == ' ')
                    {
                        hunter_moves(fds,cor_of_fd,grid,i,j,x,y,z);
                        printf("HUNTER-MOVED\n");
                        map_updated=1;
                    }				        		
                }
                else if(being_type == 'P') // PREY REQUESTS
                {
                    if(tile_type == 'H') 
                    {
                        cant_move(grid,i,j);
                        printf("PREY-CANT-MOVED\n");

                    }
                    else if(tile_type == 'X')
                    {
                        cant_move(grid,i,j);
                        printf("PREY-CANT-MOVED\n");

                    }
                    else if(tile_type == 'P')
                    {
                        cant_move(grid,i,j);
                        printf("PREY-CANT-MOVED\n");
                    
                    }
                    else if(tile_type == ' ')
                    {
                        prey_moves(cor_of_fd,grid,i,j,x,y);
                        map_updated=1;
                        printf("PREY-MOVED\n");
                    	
                    }							        	
                }
            }
		    if(map_updated)
		    {
		        print_grid(grid,height,width);
		    }
    	}
	        if(no_h == 0 || no_p == 0)
	        {
	            break;
	        }
    }
	massacre(grid);
    return 0;
    
}
