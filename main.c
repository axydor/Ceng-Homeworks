#include "do_not_submit.h"
#include <pthread.h>
#include <semaphore.h>
    
static pthread_mutex_t mutexes[ GRIDSIZE ][ GRIDSIZE ];
static pthread_mutex_t wakeup_mut  = PTHREAD_MUTEX_INITIALIZER; // Mutex for WakeUp Condition variable
static pthread_cond_t  wakeup_cond = PTHREAD_COND_INITIALIZER;  // Condition Variable for Setting the Number Of Sleeping Ants
static pthread_mutex_t delay_mut  = PTHREAD_MUTEX_INITIALIZER; // Mutex for WakeUp Condition variable
static pthread_mutex_t grid_mut  = PTHREAD_MUTEX_INITIALIZER; // Mutex for WakeUp Condition variable
static pthread_mutex_t end_mut  = PTHREAD_MUTEX_INITIALIZER; // Mutex for WakeUp Condition variable
//static pthread_mutex_t start_mut   = PTHREAD_MUTEX_INITIALIZER; // Mutex for Start Condition variable
//static pthread_cond_t  start_cond  = PTHREAD_COND_INITIALIZER;  // Condition Variable for Starting Each Thread at the same time.
int end_flag = 0;

struct Ant {
    int id; // Id of the Ant
    int x;  // X coordinate of the Ant 
    int y;  // Y coordinate of the Ant
    char state;
    };

// STATES 
// 'P': WITH FOOD, '1': WITHOUT FOOD, 'T': TIRED, 'S': SLEEPING, '$': SLEEPING WITH FOOD

void safe_unlock(int x, int y, int a, int b)
{
    pthread_mutex_unlock( &mutexes[x][y] );
    pthread_mutex_unlock( &mutexes[a][b] );
}

void antMoves(void* Y, int a, int b)
{
    struct Ant* Ant = (struct Ant*) Y;
    if( Ant-> state == '1' )  // WITHOUT FOOD
    {   
        putCharTo( Ant->x, Ant->y, '-' );
        putCharTo ( a, b, 'P' );
        Ant->state = 'P';    
    }
    else if( Ant->state == 'P' ) // WITH FOOD
    {
        putCharTo( Ant->x, Ant->y, 'o' );
        putCharTo ( a, b, '1' );                
        Ant->state = 'T';        
    }
    else if( Ant->state == 'T' ) // TIRED
    {
        putCharTo( Ant->x, Ant->y, '-' );
        putCharTo ( a, b, '1' );                
        Ant->state = '1';               
    }
    Ant->x = a;
    Ant->y = b;

}
//   POS
// |1 2 3|
// |4 5 6|
// |7 8 9|
// 
void safe_putCharTo(int x, int y, char c)
{
    pthread_mutex_lock( &mutexes[x][y] );
    putCharTo( x, y, c);
    pthread_mutex_unlock( &mutexes[x][y] );
}

void search( void* Y )
{
    struct Ant* Ant = (struct Ant*) Y;
    int x = Ant->x;
    int y = Ant->y;
    char destination;
    int pos;

    if( Ant-> state == '1' )
        destination = 'o'; // SEARCH FOR FOOD
    if( Ant->state == 'P' || Ant->state == 'T')
        destination = '-'; // SEARCH FOR EMPTY CELL

    if( (x == 0) && (y == 0) ) // Ant is at the Top-Left Corner POS: 1
        pos = 1;
    else if( (x == 0) && ( (0 < y) && (y < GRIDSIZE-1) ) ) // Ant is at the Top border POS: 2
        pos = 2;
    else if( (x == 0) && (y == GRIDSIZE-1) ) // POS: 3
        pos = 3;
    else if( ( (0 < x) && x < GRIDSIZE-1) && (y == 0) )  // POS: 4
        pos = 4;
    else if( ( (0 < x) && ( x < GRIDSIZE-1) ) && ( (0 < y) && ( y < GRIDSIZE-1) ) )  // POS: 5
        pos = 5;
    else if( ( (0 < x) && ( x < GRIDSIZE-1) )&&  (y == GRIDSIZE-1) )  // POS: 6
        pos = 6;
    else if(  (x == GRIDSIZE-1) &&  (y == 0) )  // POS: 7
        pos = 7;
    else if( (x == GRIDSIZE-1) && ( (0 < y) && (y < GRIDSIZE-1) ) )  // POS: 8
        pos = 8;
    else if( (x == GRIDSIZE-1) &&  (y == GRIDSIZE-1) )  // POS: 9
        pos = 9;
    else
        printf("SOME * THING HAPPENED \n");

    int counter;
    if(pos == 1 || pos == 3 || pos == 7 || pos == 9)
        counter = 3;
    else if(pos == 2 || pos == 4 || pos == 6 || pos == 8)
        counter = 5;
    else // pos == 5
        counter = 8;    

    while(counter)
    {
        if( pos == 5 || pos == 6 || pos == 8 || pos == 9 ) // UPLEFT
        {
            if( pthread_mutex_trylock( &mutexes[x-1][y-1]) == 0 )
            {
                counter--;
                if( lookCharAt( x-1, y-1 ) == destination ) // looks at Up-Left
                {
                    pthread_mutex_lock( &mutexes[x][y] );
                    antMoves( Ant, x-1, y-1 );
                    safe_unlock(x-1, y-1, x, y);
                    break;            
                }            
                pthread_mutex_unlock( &mutexes[x-1][y-1]);
                pthread_yield();                
            }         
        }    
        if( 3 < pos ) // LOOK UP
        {
            if( pthread_mutex_trylock( &mutexes[x-1][y]) == 0 )
            {
                counter--;
                if( lookCharAt( x-1, y ) == destination ) // looks at Up
                {
                    pthread_mutex_lock( &mutexes[x][y] );
                    antMoves( Ant, x-1, y );
                    safe_unlock(x-1, y, x, y);
                    break;            
                }                
                pthread_mutex_unlock( &mutexes[x-1][y]);
                pthread_yield();                
            }
        }
        if( 3 < pos && (pos != 6 ) && (pos != 9) ) // UP-RIGHT
        {
            if( pthread_mutex_trylock( &mutexes[x-1][y+1]) == 0 )
            {
                counter--;
                if( lookCharAt( x-1, y+1 ) == destination ) // looks at Up-Right
                {
                    pthread_mutex_lock( &mutexes[x][y] );
                    antMoves( Ant, x-1, y+1 );
                    safe_unlock(x-1, y+1, x, y);
                    break;            
                }                
                pthread_mutex_unlock( &mutexes[x-1][y+1]);
                pthread_yield();                
            }
        }
        if( (pos-1) % 3 != 0 ) // LOOK LEFT  // WHEN POS IS NOT 1,4,7
        {
            if( pthread_mutex_trylock( &mutexes[x][y-1]) == 0 )
            {
                counter--;
                if( lookCharAt( x, y-1 ) == destination ) // looks at Left
                {
                    pthread_mutex_lock( &mutexes[x][y] );
                    antMoves( Ant, x, y-1 );
                    safe_unlock(x, y-1, x, y);
                    break;            
                }                
                pthread_mutex_unlock( &mutexes[x][y-1]);
                pthread_yield();
            }            
        }        
        if( pos % 3 != 0) // RIGHT // If pos != 3,6,9
        {
            pthread_mutex_lock( &mutexes[x][y] );
            if( pthread_mutex_trylock( &mutexes[x][y+1]) == 0 )
            {
                counter--;
                if( lookCharAt( x, y+1 ) == destination ) // Ant looks at Right
                {
                    antMoves( Ant, x, y+1 );
                    safe_unlock(x, y, x, y+1);
                    break;
                }
                pthread_mutex_unlock( &mutexes[x][y+1]);
                pthread_yield();
            }
            pthread_mutex_unlock( &mutexes[x][y] );

        }
        if( pos == 2 || pos == 3 || pos == 5 || pos == 6 ) // DOWN-LEFT
        {
            pthread_mutex_lock( &mutexes[x][y] );
            if( pthread_mutex_trylock( &mutexes[x+1][y-1]) == 0 )
            {
                counter--;
                if( lookCharAt( x+1, y-1 ) == destination ) // looks at Down-Left
                {
                    antMoves( Ant, x+1, y-1 );
                    safe_unlock(x, y, x+1, y-1);
                    break;            
                }                
                pthread_mutex_unlock( &mutexes[x+1][y-1]);
                pthread_yield();
            }
            pthread_mutex_unlock( &mutexes[x][y] );
        }
        if( pos < 7 ) // DOWN
        {
            pthread_mutex_lock( &mutexes[x][y] );
            if( pthread_mutex_trylock( &mutexes[x+1][y]) == 0 )
            {
                counter--;
                if( lookCharAt( x+1, y ) == destination ) // looks at Down
                {
                    antMoves( Ant, x+1, y );
                    safe_unlock(x, y, x+1, y);
                    break;            
                }                
                pthread_mutex_unlock( &mutexes[x+1][y]);
                pthread_yield();
            }
            pthread_mutex_unlock( &mutexes[x][y] );
        }
        if( pos < 6 && (pos != 3) ) // DOWN-RIGHT
        {
            pthread_mutex_lock( &mutexes[x][y] );
            if( pthread_mutex_trylock( &mutexes[x+1][y+1]) == 0 )
            {
                counter--;
                if( lookCharAt( x+1, y+1 ) == destination ) // looks at Down-Right
                {
                    antMoves( Ant, x+1, y+1 );
                    safe_unlock(x, y, x+1, y+1);
                    break;            
                }                
                pthread_mutex_unlock( &mutexes[x+1][y+1]);
                pthread_yield();
            }
            pthread_mutex_unlock( &mutexes[x][y] );
        }
    } 
}
void *Atom(void* y)
{
    struct Ant* Ant = (struct Ant*) y;

    int delay;
    while(1)
    {
        search(Ant);
        
        pthread_mutex_lock( &delay_mut );
        delay = getDelay();
        pthread_mutex_unlock( &delay_mut );
        
        usleep( delay * 1000 + (rand() % 5000));
        
        pthread_mutex_lock( &wakeup_mut );
        while( Ant->id < getSleeperN() )
        {
            // LOCK THE CELL AND PUTCHAR 'S'
            if( Ant->state == '1' || Ant->state == 'T' ) // WITHOUT FOOD 
                safe_putCharTo( Ant->x, Ant->y, 'S' );
            else
                safe_putCharTo( Ant->x, Ant->y, '$' );

            pthread_cond_wait( &wakeup_cond, &wakeup_mut );
            // woke up

        }
            if( Ant->state == '1' || Ant->state == 'T' ) // WITHOUT FOOD 
                safe_putCharTo( Ant->x, Ant->y, '1' );
            else
                safe_putCharTo( Ant->x, Ant->y, 'P' );                
        pthread_mutex_unlock( &wakeup_mut );
        
        pthread_mutex_lock( &end_mut );
        if( end_flag == 1)
        {
            pthread_mutex_unlock( &end_mut );
            break;
        }
        pthread_mutex_unlock( &end_mut );

    }
    return NULL;
}

int main(int argc, char *argv[]) {
    srand(time(NULL));
    int n_ants, n_foods, n_time;
    n_ants  = atoi( argv[1] );
    n_foods = atoi( argv[2] );
    n_time  = atoi( argv[3] );

    struct Ant* Ants = malloc(sizeof(struct Ant) * n_ants );
    pthread_t threads[ n_ants ];

    int i,j;
    for (i = 0; i < GRIDSIZE; i++) {
        for (j = 0; j < GRIDSIZE; j++) {
            pthread_mutex_init( &mutexes[i][j], NULL );
            putCharTo(i, j, '-');
        }
    }
    int a,b;
    for (i = 0; i < n_foods; i++) {
        do {
            a = rand() % GRIDSIZE;
            b = rand() % GRIDSIZE;
        }while (lookCharAt(a,b) != '-'  );
        putCharTo(a, b, 'o');
    }
    char c;
    startCurses();

    for(i=0; i < n_ants; i++)
    {
        do {
            a = rand() % GRIDSIZE;
            b = rand() % GRIDSIZE;
        }while (lookCharAt(a,b) != '-');
        putCharTo(a, b, '1');
        Ants[i].id = i;
        Ants[i].x  = a;
        Ants[i].y  = b;
        Ants[i].state = '1';
        pthread_create( &threads[i], NULL, Atom, (void *) &Ants[i] );
    }

    pthread_mutex_lock( &wakeup_mut );
    int sleepers = getSleeperN();
    pthread_mutex_unlock( &wakeup_mut );


    struct timespec start, stop, initial;
    clock_gettime(CLOCK_MONOTONIC, &start);
    clock_gettime(CLOCK_MONOTONIC, &initial);
    
    while (TRUE) {

        // TIME TO KILL ANTS
        clock_gettime(CLOCK_MONOTONIC, &stop);
        if( (stop.tv_sec - initial.tv_sec ) == n_time ) 
        {
            pthread_mutex_lock( &end_mut );
            end_flag = 1;
            pthread_mutex_unlock( &end_mut );
            break;
        }
        // IT'S TIME TO CALL DRAW
        //printf("%lf\n", (stop.tv_nsec - start.tv_nsec) / 1.0e3 );
        //usleep(100000);
        if( (stop.tv_sec - start.tv_sec) * 1e6 + (stop.tv_nsec - start.tv_nsec) / 1.0e3  >= (DRAWDELAY) )
        {
            for(i=0; i < GRIDSIZE; i++)
            {
                for(j=0; j < GRIDSIZE; j++)
                {
                    pthread_mutex_lock( &mutexes[i][j] );
                }
            }
            clock_gettime(CLOCK_MONOTONIC, &start);
            drawWindow();
        
            for(i=0; i < GRIDSIZE; i++)
            {
                for(j=0; j < GRIDSIZE; j++)
                {
                    pthread_mutex_unlock( &mutexes[i][j] );
                }
            }
        }

        pthread_mutex_lock( &wakeup_mut );
        while( sleepers > getSleeperN())
        {
            sleepers--;
            pthread_cond_broadcast( &wakeup_cond );
        }
        sleepers = getSleeperN();
        pthread_mutex_unlock( &wakeup_mut );

        c = 0;
        c = getch();

        if (c == 'q' || c == ESC)
        {
            pthread_mutex_lock( &end_mut );
            end_flag = 1;
            pthread_mutex_unlock( &end_mut );
            break;
        } 
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

    for(i=0; i < n_ants; i++)
    {
        (void) pthread_join( threads[i],NULL );
    }
    
    // do not forget freeing the resources you get
    endCurses();
    free(Ants);
    return 0;
}
