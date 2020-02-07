
#include "../vector.h"
#include <pthread.h>


struct thread_info {
    pthread_t thread_id;
    int thread_num;
    char *argv_string;
};

#define READERS     ( 10 )
#define WRITERS     ( 2 )
#define THREADS     ( READERS + WRITERS )

struct thread_info thread_array[THREADS];


void reader_thread()
{

}

void writer_thread()
{

}

int main(void)
{
    int res;
    
    pthread_attr_t thrd_attr;
    res = pthread_attr_init(&thrd_attr);

    for ( int i = 0; i < READERS; i++) {
        int pid = pthread_create(&thread_array[i].thread_id,
                                    NULL,
                                    &thread_start,
                                    &tinfo[i]);
    }

    for ( int i = READERS; i < THREADS; i++) {
        int pid = pthread_create(&thread_array[i].thread_id,
                                    NULL,
                                    &thread_start,
                                    &tinfo[i]);
    }

}