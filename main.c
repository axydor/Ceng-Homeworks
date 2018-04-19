#include "do_not_submit.h"
#include <pthread.h>
#include <semaphore.h>
    
    static pthread_mutex_t wakeup_mut = PTHREAD_MUTEX_INITIALIZER;
    static pthread_cond_t wakeup_cond = PTHREAD_COND_INITIALIZER;

    struct A{
    	int id;
        int x;
        int y;   
        pthread_mutex_t* draw_mut;
        };
 
void *Ant(void* y)
{
    struct A* x= (struct A*)y;
    char state = '1';
    while(1)
    {
        if( pthread_mutex_lock( x->draw_mut ) == 0);
        {
        if( state == '1' ) // Ant is w.o food
        {
            putCharTo(x->x, x->y, 'o');
            x->x++;
            x->y++;
        }
        drawWindow();
        usleep(getDelay() * 1000 + (rand() % 5000));
        pthread_mutex_unlock( x->draw_mut );
        if( x->x == 25 )
            break;
        }

        pthread_mutex_lock( &wakeup_mut );
        while( x->id < getSleeperN() )
        {
        	pthread_cond_wait( &wakeup_cond, &wakeup_mut );
        }
        pthread_mutex_unlock( &wakeup_mut );

    }
    while(1);
}

int main(int argc, char *argv[]) {
    srand(time(NULL));
    struct A* A=malloc(sizeof(struct A));
    A->id = 0;
    A->x = 0;
    A->y = 0;
    int n_ants, n_foods, n_time;
    pthread_t thread1;
    pthread_mutex_t draw_mut;
    pthread_mutex_init( &draw_mut, NULL);
    A->draw_mut = &draw_mut;
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
    pthread_create( &thread1, NULL, Ant,(void *) A);
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
    setSleeperN(1);
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
