#include "do_not_submit.h"
#include <pthread.h>
#include <semaphore.h>
    
static pthread_mutex_t wakeup_mut = PTHREAD_MUTEX_INITIALIZER; // Mutex for WakeUp Condition variable
static pthread_cond_t wakeup_cond = PTHREAD_COND_INITIALIZER;  // Condition Variable for Setting the Number Of Sleeping Ants
static pthread_mutex_t draw_mut   = PTHREAD_MUTEX_INITIALIZER; // Mutex for drawWindow() call

struct Ant {
   	int id; // Id of the Ant
    int x;  // X coordinate of the Ant 
    int y;  // Y coordinate of the Ant
    char state;
    };

void withoutFood(void* Y, int a, int b)
{
    struct Ant* Ant = (struct Ant*) Y;
    if( Ant-> state == '1' )
    {   
        putCharTo( Ant->x, Ant->y, '-' );
        putCharTo ( a, b, 'P' );
        Ant->state = 'P';    
    }
    if( Ant->state == 'P' )
    {
        putCharTo( Ant->x, Ant->y, 'o' );
        putCharTo ( a, b, '1' );                
        Ant->state = '1';        
    }
    Ant->x = a;
    Ant->y = b;
}

void search( void* Y )
{
    struct Ant* Ant = (struct Ant*) Y;
    char destination;
    if( Ant-> state == '1' )
    {   
        destination == 'o';
    }
    if( Ant->state == 'P' )
    {
        destination == '-';
    }
    if( Ant->x == 0 && Ant->y == 0 ) // Ant is at the Top-Left Corner
    {
        if( lookCharAt( Ant->x, Ant->y+1 ) == destination ) // Ant looks at Right
        {
            withoutFood( Ant, Ant->x, Ant->y+1 );
        }
        else if( lookCharAt( Ant->x+1, Ant->y ) == destination )
        {
            withoutFood( Ant, Ant->x+1, Ant->y );
        }
        else if( lookCharAt( Ant->x+1, Ant->y+1 ) == destination )
        {
            withoutFood( Ant, Ant->x+1, Ant->y+1 );
        }
        pthread_mutex_lock( &draw_mut );
        drawWindow();
        pthread_mutex_unlock( &draw_mut );
        usleep(getDelay() * 1000 + (rand() % 5000));  // Should I use mutex for DELAY?
    }
    else if( Ant->x == 0 && 0 < Ant->y < GRIDSIZE-1 ) // Ant is at the Top border POS: 2
    {
        if( lookCharAt( Ant->x, Ant->y+1 ) == destination ) // Ant looks at Right
        {
            withoutFood( Ant, Ant->x, Ant->y+1 );
        }
        else if( lookCharAt( Ant->x, Ant->y-1 ) == destination )
        {
            withoutFood( Ant, Ant->x, Ant->y-1 );
        }                
        else if( lookCharAt( Ant->x+1, Ant->y-1 ) == destination ) // looks at Down
        {
            withoutFood( Ant, Ant->x+1, Ant->y-1 );
        }
        else if( lookCharAt( Ant->x+1, Ant->y ) == destination ) // looks at down right
        {
            withoutFood( Ant, Ant->x+1, Ant->y );
        }
        else if( lookCharAt( Ant->x+1, Ant->y+1 ) == destination )
        {
            withoutFood( Ant, Ant->x+1, Ant->y+1 );
        }                              
        pthread_mutex_lock( &draw_mut );
        drawWindow();
        pthread_mutex_unlock( &draw_mut );
        usleep(getDelay() * 1000 + (rand() % 5000));  // Should I use mutex for DELAY?               
    } 
    else if( Ant->x == 0 && Ant->y == GRIDSIZE-1 ) // POS: 3
    {
        if( lookCharAt( Ant->x, Ant->y-1 ) == destination ) // Ant looks at Left
        {
            withoutFood( Ant, Ant->x, Ant->y-1 );
        }
        else if( lookCharAt( Ant->x+1, Ant->y-1 ) == destination )
        {
            withoutFood( Ant, Ant->x+1, Ant->y-1 );
        }                
        else if( lookCharAt( Ant->x+1, Ant->y ) == destination ) // looks at Down
        {
            withoutFood( Ant, Ant->x+1, Ant->y );
        }
        pthread_mutex_lock( &draw_mut );
        drawWindow();
        pthread_mutex_unlock( &draw_mut );
        usleep(getDelay() * 1000 + (rand() % 5000));  // Should I use mutex for DELAY?                  ;
    }
    else if( 0 < Ant->x < GRIDSIZE-1 && Ant->y == 0 )  // POS: 4
    {
        if( lookCharAt( Ant->x-1, Ant->y ) == destination ) // UP
        {
            withoutFood( Ant, Ant->x-1, Ant->y );
        }

        else if( lookCharAt( Ant->x-1, Ant->y+1 ) == destination ) // UP-RIGHT
        {
            withoutFood( Ant, Ant->x-1, Ant->y+1 );
        }  

        else if( lookCharAt( Ant->x, Ant->y+1 ) == destination ) // RIGHT
        {
            withoutFood( Ant, Ant->x+1, Ant->y-1 );
        }
        
        else if( lookCharAt( Ant->x+1, Ant->y+1 ) == destination ) // DOWN-RIGHT
        {
            withoutFood( Ant, Ant->x+1, Ant->y+1 );
        }

        else if( lookCharAt( Ant->x+1, Ant->y ) == destination ) // DOWN
        {
            withoutFood( Ant, Ant->x+1, Ant->y );
        }


        pthread_mutex_lock( &draw_mut );
        drawWindow();
        pthread_mutex_unlock( &draw_mut );
        usleep(getDelay() * 1000 + (rand() % 5000));  // Should I use mutex for DELAY?                     
    }
    else if( 0 < Ant->x < GRIDSIZE-1 && 0 < Ant->y < GRIDSIZE-1 )  // POS: 5
    {
        if( lookCharAt( Ant->x-1, Ant->y ) == destination ) // UP
        {
            withoutFood( Ant, Ant->x-1, Ant->y );
        }

        else if( lookCharAt( Ant->x-1, Ant->y+1 ) == destination ) // UP-RIGHT
        {
            withoutFood( Ant, Ant->x-1, Ant->y+1 );
        }  

        else if( lookCharAt( Ant->x, Ant->y+1 ) == destination ) // RIGHT
        {
            withoutFood( Ant, Ant->x, Ant->y+1 );
        }

        else if( lookCharAt( Ant->x+1, Ant->y+1 ) == destination ) // DOWN-RIGHT
        {
            withoutFood( Ant, Ant->x+1, Ant->y+1 );
        }
        
        else if( lookCharAt( Ant->x+1, Ant->y ) == destination ) // DOWN
        {
            withoutFood( Ant, Ant->x+1, Ant->y );
        }

        else if( lookCharAt( Ant->x+1, Ant->y-1 ) == destination ) // DOWN-LEFT
        {
            withoutFood( Ant, Ant->x+1, Ant->y-1 );
        }

        else if( lookCharAt( Ant->x, Ant->y-1 ) == destination ) // LEFT
        {
            withoutFood( Ant, Ant->x, Ant->y-1 );
        }

        else if( lookCharAt( Ant->x-1, Ant->y-1 ) == destination ) // UP-LEFT
        {
            withoutFood( Ant, Ant->x-1, Ant->y-1 );
        }
        pthread_mutex_lock( &draw_mut );
        drawWindow();
        pthread_mutex_unlock( &draw_mut );
        usleep(getDelay() * 1000 + (rand() % 5000));  // Should I use mutex for DELAY?                     
    }

    else if( 0 < Ant->x < GRIDSIZE-1 &&  Ant->y == GRIDSIZE-1 )  // POS: 6
    {
        if( lookCharAt( Ant->x-1, Ant->y ) == destination ) // UP
        {
            withoutFood( Ant, Ant->x-1, Ant->y );
        }

        else if( lookCharAt( Ant->x+1, Ant->y ) == destination ) // DOWN
        {
            withoutFood( Ant, Ant->x+1, Ant->y );
        }

        else if( lookCharAt( Ant->x+1, Ant->y-1 ) == destination ) // DOWN-LEFT
        {
            withoutFood( Ant, Ant->x+1, Ant->y-1 );
        }

        else if( lookCharAt( Ant->x, Ant->y-1 ) == destination ) // LEFT
        {
            withoutFood( Ant, Ant->x, Ant->y-1 );
        }

        else if( lookCharAt( Ant->x-1, Ant->y-1 ) == destination ) // UP-LEFT
        {
            withoutFood( Ant, Ant->x-1, Ant->y-1 );
        }
        pthread_mutex_lock( &draw_mut );
        drawWindow();
        pthread_mutex_unlock( &draw_mut );
        usleep(getDelay() * 1000 + (rand() % 5000));  // Should I use mutex for DELAY?                     
    }            

    else if(  Ant->x == GRIDSIZE-1 &&  Ant->y ==0 )  // POS: 7
    {
        if( lookCharAt( Ant->x-1, Ant->y ) == destination ) // UP
        {
            withoutFood( Ant, Ant->x-1, Ant->y );
        }

        else if( lookCharAt( Ant->x-1, Ant->y+1 ) == destination ) // UP-RIGHT
        {
            withoutFood( Ant, Ant->x-1, Ant->y+1 );
        }  

        else if( lookCharAt( Ant->x, Ant->y+1 ) == destination ) // RIGHT
        {
            withoutFood( Ant, Ant->x, Ant->y+1 );
        }
        pthread_mutex_lock( &draw_mut );
        drawWindow();
        pthread_mutex_unlock( &draw_mut );
        usleep(getDelay() * 1000 + (rand() % 5000));  // Should I use mutex for DELAY?                     
    }

    else if( Ant->x == GRIDSIZE-1 && 0 < Ant->y < GRIDSIZE-1 )  // POS: 8
    {
        if( lookCharAt( Ant->x-1, Ant->y ) == destination ) // UP
        {
            withoutFood( Ant, Ant->x-1, Ant->y );
        }

        else if( lookCharAt( Ant->x-1, Ant->y+1 ) == destination ) // UP-RIGHT
        {
            withoutFood( Ant, Ant->x-1, Ant->y+1 );
        }  

        else if( lookCharAt( Ant->x, Ant->y+1 ) == destination ) // RIGHT
        {
            withoutFood( Ant, Ant->x, Ant->y+1 );
        }

        else if( lookCharAt( Ant->x, Ant->y-1 ) == destination ) // LEFT
        {
            withoutFood( Ant, Ant->x, Ant->y-1 );
        }

        else if( lookCharAt( Ant->x-1, Ant->y-1 ) == destination ) // UP-LEFT
        {
            withoutFood( Ant, Ant->x-1, Ant->y-1 );
        }
        pthread_mutex_lock( &draw_mut );
        drawWindow();
        pthread_mutex_unlock( &draw_mut );
        usleep(getDelay() * 1000 + (rand() % 5000));  // Should I use mutex for DELAY?                     
    }   

    else if(  Ant->x == GRIDSIZE-1 &&  Ant->y == GRIDSIZE-1 )  // POS: 9
    {
        if( lookCharAt( Ant->x-1, Ant->y ) == destination ) // UP
        {
            withoutFood( Ant, Ant->x-1, Ant->y );
        }

        else if( lookCharAt( Ant->x, Ant->y-1 ) == destination ) // LEFT
        {
            withoutFood( Ant, Ant->x, Ant->y-1 );
        }

        else if( lookCharAt( Ant->x-1, Ant->y-1 ) == destination ) // UP-LEFT
        {
            withoutFood( Ant, Ant->x-1, Ant->y-1 );
        }
        pthread_mutex_lock( &draw_mut );
        drawWindow();
        pthread_mutex_unlock( &draw_mut );
        usleep(getDelay() * 1000 + (rand() % 5000));  // Should I use mutex for DELAY?                     
    }                     

    pthread_mutex_lock( &wakeup_mut );
    while( Ant->id < getSleeperN() )
    {
        pthread_cond_wait( &wakeup_cond, &wakeup_mut );
    }
    pthread_mutex_unlock( &wakeup_mut );

}

//  _____
// |1 2 3|
// |4 5 6|
// |7 8 9|
// 
                                                            /////////////////////////////////////
                                                    // SHOULD I DRAW EVEN IF ANT DO NOT MOVES ?
                                                            /////////////////////////////////////

void *Atom(void* y)
{
    struct Ant* Ant = (struct Ant*)y;
    Ant->state = '1';
    while(1)
    {
        if( Ant->state == '1' ) // Ant is searching for the food 
        {
            search(Ant);
        }
        else if( Ant->state == 'P') // Ant is with food :)
        {

        }
        else if( Ant->state == 'T' ) // Ant is tired(?)
        {

        }    
    }
    while(1);  // MAKE SURE THREAD DOES NOT EXECUT EXIT
}

int main(int argc, char *argv[]) {
    srand(time(NULL));
    struct Ant* A=malloc(sizeof(struct Ant));
    A->id = 0;
    A->x = 4;
    A->y = 4;
    int n_ants, n_foods, n_time;
    pthread_t thread1;
/*    
    n_ants  = atoi( argv[1] );
    n_foods = atoi( argv[2] );
    n_time  = atoi( argv[3] );

    pthread_t thread_id[ n_ants ];
    pthread_mutex_t mutexes[ GRIDSIZE ];
    int k;
    for(k=0; k < GRIDSIZE; k++)
    {
        pthread_mutex_init( &mutexes[k], NULL );
    }
*/
    //////////////////////////////
    // Fills the grid randomly to have somthing to draw on screen.
    // Empty spaces have to be -.
    // You should get the number of ants and foods from the arguments 
    // and make sure that a food and an ant does not placed at the same cell.
    // You must use putCharTo() and lookCharAt() to access/change the grid.
    // You should be delegating ants to separate threads
    int i,j;
    for (i = 0; i < GRIDSIZE; i++) {
        for (j = 0; j < GRIDSIZE; j++) {
            putCharTo(i, j, '-');
        }
    }
    putCharTo( 4, 5, 'o');
    pthread_create( &thread1, NULL, Atom, (void *) A);
    /*
    int a,b;
    for (i = 0; i < 5; i++) {
        do {
            a = rand() % GRIDSIZE;
            b = rand() % GRIDSIZE;
        }while (lookCharAt(a,b)!= '-');
        putCharTo(a, b, 'P');
    }
    for (i = 0; i < 5; i++) {
        do {
            a = rand() % GRIDSIZE;
            b = rand() % GRIDSIZE;
        }while (lookCharAt(a,b) != '-');
        putCharTo(a, b, '1');
    }
    for (i = 0; i < 5; i++) {
        do {
            a = rand() % GRIDSIZE;
            b = rand() % GRIDSIZE;
        }while (lookCharAt(a,b) != '-');
        putCharTo(a, b, 'o');
    }
    for (i = 0; i < 5; i++) {
        do {
            a = rand() % GRIDSIZE;
            b = rand() % GRIDSIZE;
        }while (lookCharAt(a,b) != '-');
        putCharTo(a, b, 'S');
    }
    for (i = 0; i < 5; i++) {
        do {
            a = rand() % GRIDSIZE;
            b = rand() % GRIDSIZE;
        }while (lookCharAt(a,b) != '-');
        putCharTo(a, b, '$');
    }
    */
    //////////////////////////////

    // you have to have following command to initialize ncurses.
    startCurses();
       
    // You can use following loop in your program. But pay attention to 
    // the function calls, they do not have any access control, you 
    // have to ensure that.
    char c;
    pthread_mutex_lock( &wakeup_mut );
    //*setSleeperN(1);
    int sleepers = getSleeperN();
    pthread_mutex_unlock( &wakeup_mut );
    while (TRUE) {
    	pthread_mutex_lock( &wakeup_mut );
    	if( sleepers != getSleeperN())
    	{
    		sleepers = getSleeperN();
    		pthread_cond_signal( &wakeup_cond );
    	}
    	pthread_mutex_unlock( &wakeup_mut );

        pthread_mutex_lock( &draw_mut );
        drawWindow();
        usleep(DRAWDELAY);
        pthread_mutex_unlock( &draw_mut );        
        c = 0;
        c = getch();

        if (c == 'q' || c == ESC) break;
        if (c == '+') {
            setDelay(getDelay()+10);
        }
        if (c == '-') {
            setDelay(getDelay()-10);
        }
        if (c == '*') {
        	    	pthread_mutex_lock( &wakeup_mut );
            setSleeperN(getSleeperN()+1);
                	pthread_mutex_unlock( &wakeup_mut );
        }
        if (c == '/') {
        	    	pthread_mutex_lock( &wakeup_mut );
            setSleeperN(getSleeperN()-1);
                	pthread_mutex_unlock( &wakeup_mut );
        }

        
        // each ant thread have to sleep with code similar to this
        //usleep(getDelay() * 1000 + (rand() % 5000));
    }
    
    // do not forget freeing the resources you get
    endCurses();
    
    return 0;
}
