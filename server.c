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

int real_no_p,real_no_h;

typedef struct coordinate
{
    int x;
    int y;    
} coordinate;

typedef struct object 
{
	char type;
	int health;
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


void print_grid(object** grid, int height, int width)
{
    int i,j,y;
    printf("+");
    fflush(stdout);
    for(y=0; y < width; y++)
    {
        printf("-");
        fflush(stdout);
    }
    printf("+\n");
    fflush(stdout);
    for(i=0; i < height; i++)
    {
        printf("|");
        fflush(stdout);

        for(j=0; j < width; j++)
        {
            printf("%c", grid[i][j].type);    
            fflush(stdout);
        }
        printf("|\n");
        fflush(stdout);
    }
    printf("+");
    fflush(stdout);
    for(y=0; y < width; y++)
    {
        printf("-");
        fflush(stdout);
    }
    printf("+\n");
    fflush(stdout);
}

void take_input(object** grid)
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
     }
    scanf("%d", &no_p);
    for(i=0; i < no_p; i++)
    {
        int x,y,health;
        scanf("%d %d %d", &x, &y, &health);
        grid[x][y].type = 'P';
        grid[x][y].health = health;
    }
}

coordinate MH(object** grid,int a,int b)
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

int no_neighbours(object** grid,int a,int b, coordinate object_pos[4] )
{
	int x = a;
	int y = b;
	char type = grid[x][y].type;
	int i=0;
	if( 0 <= x - 1 )
	{
		if(grid[x-1][y].type == 'X' || grid[x-1][y].type == type )  // UP
		{
			object_pos[i].x = x-1;
			object_pos[i].y = y;
			i++;
		}
	}
	if( x+1 < height)  //  DOWN
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

void hunter_kills_prey(struct pollfd fds[no_h+no_p],coordinate cor_of_fd[no_h+no_p],object** grid,int i,int j,int x,int y,int z)
{
    //printf("HUNTER KILS PREY\n");
    //fflush(stdout);

	int a=0;
	int h_fd_i = z;
	int p_fd_i;

	for(a=0; a < no_h + no_p;a++)
	{
		if(fds[a].fd == grid[x][y].fd) // FIND PREY'S FILE DESCRIPTOR
		{
			p_fd_i = a;
            break;
		}
	}

	int pid_prey = grid[x][y].pid;
    close(grid[x][y].fd);

    fds[p_fd_i].fd = -1;
    cor_of_fd[p_fd_i].x = -1;
    cor_of_fd[p_fd_i].y = -1;

    kill(pid_prey,SIGTERM);
    waitpid(pid_prey,NULL,0);
    real_no_p--;

    grid[x][y].type = 'H';
    grid[x][y].fd = grid[i][j].fd;
    grid[x][y].pid = grid[i][j].pid;
    grid[x][y].health += grid[i][j].health - 1;
	cor_of_fd[h_fd_i].x = x;
	cor_of_fd[h_fd_i].y = y;
    
    grid[i][j].type = ' ';
    grid[i][j].fd = -1;
    grid[i][j].pid = -1;
    grid[i][j].health = 0;
    server_message s_m;
    memset(&s_m,0,sizeof(server_message));
    memset(&s_m.adv_pos,0,sizeof(coordinate));
    memset(s_m.object_pos,0,sizeof(coordinate)*4);
    s_m.pos.x = x;
    s_m.pos.y = y;
    s_m.adv_pos = MH(grid, x, y); // LOCATION OF CLOSES ADVERSARY
    s_m.object_count =  no_neighbours(grid,x,y,s_m.object_pos);// NUMBER OF NEIGHBOURING OBJECT
    if(write(grid[x][y].fd,&s_m,sizeof(server_message)) == -1 )
        perror("HUNTER-KILLS-PREY-ERRO\n");
    
}

void cant_move(object** grid,int i,int j)
{
    server_message s_m;
    memset(&s_m,0,sizeof(server_message));
    s_m.pos.x = i;
    s_m.pos.y = j;
    s_m.adv_pos = MH(grid, i, j); // LOCATION OF CLOSES ADVERSARY
    s_m.object_count =  no_neighbours(grid,i,j,s_m.object_pos);// NUMBER OF NEIGHBOURING OBJECT
    if(write(grid[i][j].fd,&s_m,sizeof(server_message)) == -1 )
        perror("CANT-MOVE\n");
}

void hunter_moves(struct pollfd* fds,coordinate* cor_of_fd,object** grid,int i,int j,int x,int y,int z)
{
	int new_health = grid[i][j].health - 1;

	//	CLEAN THE PREVIOUS TILE

	int fd = fds[z].fd;

	if(new_health == 0)
	{
		int pid_hunter = grid[i][j].pid;
		grid[i][j].type = ' ';
		grid[i][j].fd=-1;
		grid[i][j].health=0;	
		fds[z].fd = -1;
		cor_of_fd[z].x = -1;
		cor_of_fd[z].y = -1;

		//close(fds[z].fd);
		kill(pid_hunter,SIGTERM);
		waitpid(pid_hunter,NULL,0);
		//printf("HUNTER DIED\n");
        real_no_h--;
	}
	else
	{
	    grid[x][y].health = new_health;
	    grid[x][y].type = 'H';
        grid[x][y].pid  = grid[i][j].pid;
        grid[x][y].fd  = grid[i][j].fd;
	    cor_of_fd[z].x = x;
	    cor_of_fd[z].y = y;

		grid[i][j].type = ' ';
        grid[i][j].health=0;    
        grid[i][j].pid=-1;    
		grid[i][j].fd=-1;	

	    server_message s_m;
	    memset(&s_m,0,sizeof(server_message));
	    s_m.pos.x = x;
	    s_m.pos.y = y;
	    s_m.adv_pos = MH(grid, x, y); // LOCATION OF CLOSES ADVERSARY
	    //printf("s_m_.adv_pos x : %d - y: %d\n", s_m.adv_pos.x,s_m.adv_pos.y);
	    //printf("HUNTER MOVED FROM %d-%d to %d,%d\n",i,j,x,y );
	    s_m.object_count =  no_neighbours(grid,x,y,s_m.object_pos);// NUMBER OF NEIGHBOURING OBJECT
	    if(write(fd,&s_m,sizeof(server_message)) < 0 )
	        printf("hunt-ERROR-MOVES\n");
	}
		
}
void prey_moves(struct pollfd* fds,coordinate cor_of_fd[no_h+no_p],object** grid,int i,int j,int x,int y)
{
	int a;
    int p_fd_i;

    for(a=0; a < no_h + no_p;a++)
    {
        if(fds[a].fd == grid[i][j].fd) // FIND PREY'S FILE DESCRIPTOR
        {
            p_fd_i = a;
            break;
        }
    }
	cor_of_fd[p_fd_i].x = x;
	cor_of_fd[p_fd_i].y = y;

    grid[x][y].type = 'P';
    grid[x][y].fd = grid[i][j].fd;
    grid[x][y].pid = grid[i][j].pid;
    grid[x][y].health = grid[i][j].health;

    
    grid[i][j].type = ' ';
    grid[i][j].fd = -1;
    grid[i][j].pid = -1;
    grid[i][j].health = 0;	

    server_message s_m;
    memset(s_m.object_pos,0,sizeof(coordinate) * 4);
    memset(&s_m,0,sizeof(server_message));
    s_m.pos.x = x;
    s_m.pos.y = y;
    s_m.adv_pos = MH(grid, x, y); // LOCATION OF CLOSEST ADVERSARY
    s_m.object_count =  no_neighbours(grid,x,y,s_m.object_pos);// NUMBER OF NEIGHBOURING OBJECT
    write(fds[p_fd_i].fd,&s_m,sizeof(server_message));

}

void massacre(object** grid)
{
	//printf("MASSACRE IS INEVITABLE\n");
    //fflush(stdout);
	int i,j;
	for(i=0; i < height; i++)
	{
		for(j=0; j < width; j++)
		{
			if(grid[i][j].type == 'H' || grid[i][j].type == 'P' )
			{
				int dip = grid[i][j].pid;
    		    //printf("TYPE:%c, LOC:%d-%d, pid:%d\n",grid[i][j].type,i,j,dip);
    		    fflush(stdout);
                close(grid[i][j].fd);
    		    kill(dip,SIGTERM);
				waitpid(dip,NULL,0);
			}
		}
	}	
}

int main()
{
	int i, j, z;
    object** grid;

    struct pollfd* fds;

    scanf("%d %d", &width, &height);
	grid = malloc(sizeof(object*) * height);
	for(i=0; i < height; i++)
	{
		grid[i] = malloc(sizeof(object) * width);
	}

	take_input(grid);
    print_grid(grid,height,width);

    real_no_h = no_h;
    real_no_p = no_p;

    fds = malloc( sizeof(struct pollfd) * (no_p+no_h) );  //	ARRAY OF FILE DESCRIPTORS

    coordinate* cor_of_fd;
    cor_of_fd = malloc(sizeof(coordinate) * (no_h+no_p));
    
    int ip=-1;            //  ARRAY OF PID, PID AND RELATED FD ARE AT THE SAME INDEX ON THE TWO ARRAYS(fds[],control[]).

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
                if( ( grid[i][j].pid = fork() ) )
                {
                    close(fd[1]);
                    fds[ip].fd       = fd[0];
                    fds[ip].events   = POLLIN;
                    grid[i][j].fd    = fd[0];

                    cor_of_fd[ip].x = i;
                    cor_of_fd[ip].y = j;
			        
			        server_message s_m;
			        memset(&s_m,0,sizeof(server_message));

			        s_m.pos.x = i;
			        s_m.pos.y = j;
			        s_m.adv_pos = MH(grid, i,j); // LOCATION OF CLOSES ADVERSARY
			        s_m.object_count =  no_neighbours(grid,i,j,s_m.object_pos);// NUMBER OF NEIGHBOURING OBJECT
			        if(write(fd[0],&s_m,sizeof(server_message)) < 0 )
			            printf("SERVER-WRITE-ERROR\n");

                }
                else  // CHILD
                {   
                    dup2(fd[1],0);
                    dup2(fd[1],1);
                    close(fd[0]);
                    close(fd[1]);

                    char s_height[10];
                    char s_width[10];

                    sprintf( s_height, "%d", height );
                    sprintf( s_width, "%d", width );
                    if(grid[i][j].type == 'H')
                    {
                        char* newargv[] = { "hunter", s_height, s_width, NULL };
                        if( execv( "hunter", newargv ) == -1)
                            write(2,"EXECVE ERROR",20);		                	
                    }
                    else
                    {
                        char* newargv[] = { "prey", s_height, s_width, NULL };
                        if( execv( "prey", newargv) == -1)
                            write(2,"EXECVE ERROR",20);		  
                    }
                }
            }

        }
    }

	int retval,map_updated;

    while(real_no_h && real_no_p)
    {
    	retval = poll(fds,no_h+no_p,0);
    	if(retval < 0 )
    	{
    		continue;
    	}

        //printf("RAMBO\n" );	
        for(z=0; z < no_p + no_h; z++)          
        {
	    // ALL FILE DESCRIPTORS OF ALIVE CHILDREN ARE READY
        	map_updated=0;
            if(fds[z].revents & POLLIN)
            {
                ph_message message;
                memset(&message,0,sizeof(ph_message));

                read(fds[z].fd, &message, sizeof(ph_message));

                int i = cor_of_fd[z].x;
                int j = cor_of_fd[z].y;

                if(i == -1 || j == -1)
                {
                    continue;
                }
                int x = message.move_request.x;
                int y = message.move_request.y;
 
                char being_type = grid[i][j].type;

                char tile_type  = grid[x][y].type;

                //printf("MOVE REQ OF %c AT THE x:%d - y:%d to x:%d - y:%d\n",being_type,i,j,x,y );

  
                if(being_type == 'H') // HUNTER REQUESTS
                {
                    if(tile_type == 'P')  // HUNTER KILSS PREY
                    {
                        map_updated=1;
                        hunter_kills_prey(fds,cor_of_fd,grid,i,j,x,y,z);
                        //printf("HUNTER-KILLS\n");
                    	
                    }
                    else if(tile_type == 'X')
                    {
                        cant_move(grid,i,j);
                        //printf("HUNTER-CANT-MOVED\n");

                    }
                    else if(tile_type == 'H')
                    {
                        cant_move(grid,i,j);
                        //printf("HUNTER-CANT-MOVED\n");

                    }
                    else if(tile_type == ' ')
                    {
                        map_updated=1;
                        hunter_moves(fds,cor_of_fd,grid,i,j,x,y,z);
                        //printf("HUNTER-MOVED\n");
                    }				        		
                }
                else if(being_type == 'P') // PREY REQUESTS
                {
                    if(tile_type == 'H') 
                    {
                        cant_move(grid,i,j);
                        //printf("PREY-CANT-MOVED\n");
                        fflush(stdout);

                    }
                    else if(tile_type == 'X')
                    {
                        cant_move(grid,i,j);
                        //printf("PREY-CANT-MOVED\n");
                        fflush(stdout);

                    }
                    else if(tile_type == 'P')
                    {
                        cant_move(grid,i,j);
                        //printf("PREY-CANT-MOVED\n");
                        fflush(stdout);
                    
                    }
                    else if(tile_type == ' ')
                    {
                        map_updated=1;
                        prey_moves(fds,cor_of_fd,grid,i,j,x,y);
                        //printf("PREY-MOVED\n");
                    	fflush(stdout);
                    }							        	
                }
                else if(being_type == ' ')
                {
                    printf("SERIOUS PROBLEM\n");
                }
            }
            if(map_updated)
            {
                print_grid(grid,height,width);
            }
        }
    }
    
	massacre(grid);
    free(fds);   
    free(cor_of_fd); 
    for(i=0; i < height; i++)
    {
        free(grid[i]);
    }
    free(grid);

    return 0;
}
