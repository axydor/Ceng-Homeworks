#include "do_not_submit.h"
#include <pthread.h>
#include <semaphore.h>
    
static pthread_mutex_t mutexes[ GRIDSIZE ][ GRIDSIZE ];
static pthread_mutex_t wakeup_mut  = PTHREAD_MUTEX_INITIALIZER; // Mutex for WakeUp Condition variable
static pthread_cond_t  wakeup_cond = PTHREAD_COND_INITIALIZER;  // Condition Variable for Setting the Number Of Sleeping Ants
static pthread_mutex_t start_mut   = PTHREAD_MUTEX_INITIALIZER; // Mutex for Start Condition variable
static pthread_cond_t  start_cond  = PTHREAD_COND_INITIALIZER;  // Condition Variable for Starting Each Thread at the same time.
static pthread_mutex_t draw_mut    = PTHREAD_MUTEX_INITIALIZER; // Mutex for drawWindow() call

struct Ant {
    int id; // Id of the Ant
    int x;  // X coordinate of the Ant 
    int y;  // Y coordinate of the Ant
    char state;
    };

// STATES 
// 'P': WITH FOOD, '1': WITHOUT FOOD, 'T': TIRED, 'S': SLEEPING, '$': SLEEPING WITH FOOD

void antMoves(void* Y, int a, int b)
{
    struct Ant* Ant = (struct Ant*) Y;
    pthread_mutex_lock( &mutexes[Ant->x][Ant->y] ); // LOCK THIS CELL
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
    pthread_mutex_unlock( &mutexes[Ant->x][Ant->y] ); // LOCK THIS CELL

}
//   POS
// |1 2 3|
// |4 5 6|
// |7 8 9|
// 
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
    else if( ( (0 < x) && ( x< GRIDSIZE-1) )&&  (y == GRIDSIZE-1) )  // POS: 6
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
        if( 3 < pos )
        {
            if( pthread_mutex_trylock( &mutexes[x-1][y]) == 0 )
            {
                counter--;
                if( lookCharAt( x-1, y ) == destination ) // looks at Up
                {
                    antMoves( Ant, x-1, y );
                    pthread_mutex_unlock( &mutexes[x-1][y]);
                    break;            
                }                
                pthread_mutex_unlock( &mutexes[x-1][y]);
            }
        }
        if( 3 < pos && (pos != 6 ) && (pos != 9) )
        {
            if( pthread_mutex_trylock( &mutexes[x-1][y+1]) == 0 )
            {
                counter--;
                if( lookCharAt( x-1, y+1 ) == destination ) // looks at Up-Right
                {
                    antMoves( Ant, x-1, y+1 );
                    pthread_mutex_unlock( &mutexes[x-1][y+1]);
                    break;            
                }                
                pthread_mutex_unlock( &mutexes[x-1][y+1]);
            }
        }
        if( pos % 3 != 0) // If pos != 3,6,9
        {
            if( pthread_mutex_trylock( &mutexes[x][y+1]) == 0 )
            {
                counter--;
                if( lookCharAt( x, y+1 ) == destination ) // Ant looks at Right
                {
                    antMoves( Ant, x, y+1 );
                    pthread_mutex_unlock( &mutexes[x][y+1]);
                    break;
                }
                pthread_mutex_unlock( &mutexes[x][y+1]);
            }
        }
        if( pos < 6 && (pos != 3) )
        {
            if( pthread_mutex_trylock( &mutexes[x+1][y+1]) == 0 )
            {
                counter--;
                if( lookCharAt( x+1, y+1 ) == destination ) // looks at Down-Right
                {
                    antMoves( Ant, x+1, y+1 );
                    pthread_mutex_unlock( &mutexes[x+1][y+1]);
                    break;            
                }                
                pthread_mutex_unlock( &mutexes[x+1][y+1]);
            }
        }
        if( pos < 7 )
        {
            if( pthread_mutex_trylock( &mutexes[x+1][y]) == 0 )
            {
                counter--;
                if( lookCharAt( x+1, y ) == destination ) // looks at Down
                {
                    antMoves( Ant, x+1, y );
                    pthread_mutex_unlock( &mutexes[x+1][y]);
                    break;            
                }                
                pthread_mutex_unlock( &mutexes[x+1][y]);
            }
        }
        if( pos == 2 || pos == 3 || pos == 5 || pos == 6 )
        {
            if( pthread_mutex_trylock( &mutexes[x+1][y-1]) == 0 )
            {
                counter--;
                if( lookCharAt( x+1, y-1 ) == destination ) // looks at Down-Left
                {
                    antMoves( Ant, x+1, y-1 );
                    pthread_mutex_unlock( &mutexes[x+1][y-1]);
                    break;            
                }                
                pthread_mutex_unlock( &mutexes[x+1][y-1]);
            }
        }
        if( (pos-1) % 3 != 0 ) // LOOK LEFT WHEN POS IS NOT 1,4,7
        {
            if( pthread_mutex_trylock( &mutexes[x][y-1]) == 0 )
            {
                counter--;
                if( lookCharAt( x, y-1 ) == destination ) // looks at Left
                {
                    antMoves( Ant, x, y-1 );
                    pthread_mutex_unlock( &mutexes[x][y-1]);
                    break;            
                }                
                pthread_mutex_unlock( &mutexes[x][y-1]);
            }            
        }
        if( pos == 5 || pos == 6 || pos == 8 || pos == 9 )
        {
            if( pthread_mutex_trylock( &mutexes[x-1][y-1]) == 0 )
            {
                counter--;
                if( lookCharAt( x-1, y-1 ) == destination ) // looks at Up-Left
                {
                    antMoves( Ant, x-1, y-1 );
                    pthread_mutex_unlock( &mutexes[x-1][y-1]);
                    break;            
                }                
                pthread_mutex_unlock( &mutexes[x-1][y-1]);
            }         
        }
    } 
}

                                                            /////////////////////////////////////
                                                    // SHOULD I DRAW EVEN IF ANT DO NOT MOVES ?
                                                            /////////////////////////////////////
void *Atom(void* y)
{
    struct Ant* Ant = (struct Ant*) y;

    // EACH ANT WAITS TO START
    pthread_mutex_lock( &start_mut );           
    pthread_cond_wait( &start_cond, &start_mut );
    pthread_mutex_unlock( &start_mut );        

    while(1)
    {
        search(Ant);
        usleep(getDelay() * 1000 + (rand() % 5000));  // Should I use mutex for DELAY?                         
        pthread_mutex_lock( &wakeup_mut );
        while( Ant->id < getSleeperN() )
        {
            // LOCK THE CELL AND PUTCHAR 'S'
            pthread_mutex_lock( &mutexes[Ant->x][Ant->y] );
            if( Ant->state == '1' || Ant->state == 'T' ) // WITHOUT FOOD 
                putCharTo( Ant->x, Ant->y, 'S' );
            else
                putCharTo( Ant->x, Ant->y, '$' );
            pthread_mutex_unlock( &mutexes[Ant->x][Ant->y] ); 

            pthread_cond_wait( &wakeup_cond, &wakeup_mut );
            // woke up

        }
            pthread_mutex_lock( &mutexes[Ant->x][Ant->y] );
            if( Ant->state == '1' || Ant->state == 'T' ) // WITHOUT FOOD 
                putCharTo( Ant->x, Ant->y, '1' );
            else
                putCharTo( Ant->x, Ant->y, 'P' );                
            pthread_mutex_unlock( &mutexes[Ant->x][Ant->y] );
        pthread_mutex_unlock( &wakeup_mut );
    }
}

int main(int argc, char *argv[]) {
    srand(time(NULL));
    int n_ants, n_foods, n_time;
    n_ants  = atoi( argv[1] );
    n_foods = atoi( argv[2] );
    n_time  = atoi( argv[3] );

    struct Ant* Ants = malloc(sizeof(struct Ant) * n_ants );
    pthread_t threads[ n_ants ];
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
    startCurses();
    // you have to have following command to initialize ncurses.
    //startCurses();
    // You can use following loop in your program. But pay attention to 
    // the function calls, they do not have any access control, you 
    // have to ensure that.
    char c;
       
    pthread_mutex_lock( &wakeup_mut );
    int sleepers = getSleeperN();
    pthread_mutex_unlock( &wakeup_mut );

    setDelay(900);
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

    pthread_mutex_lock( &start_mut );
    pthread_cond_broadcast( &start_cond );
    pthread_mutex_unlock( &start_mut );


    while (TRUE) {

        drawWindow();
        usleep(DRAWDELAY);

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
    free(Ants);
    return 0;
}
