 
#include "../vector.h"
#include <pthread.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>


struct thread_info {
    pthread_t thread_id;
    int thread_num;
    char *argv_string;
};

#define READERS     ( 2 )
#define WRITERS     ( 2 )
#define THREADS     ( READERS + WRITERS )

struct thread_info thread_array[THREADS];


void* reader_thread(void *arg)
{
    while (1) {
        printf("Reader thread pid: %d, items: %d\n", pthread_self(),
        vector_get_size( (Vector*)arg ));
        sleep(5);
    }
}

void* writer_thread(void *arg)
{
    int init = 0;
    while (1) {
        printf("Writer thread pid: %d\n", pthread_self());
        if ( vector_get_size(((struct vector*)arg)) < 10 ) {
            vector_put( (Vector*)arg, &init);
            // This is not atomic so fuck knows what will happen...
            init++;
        }
        sleep(5);
    }
}

int main(void)
{
    int res;
    
    Vector *test_vector;

    vector_construct(&test_vector, sizeof(int));

    pthread_attr_t thrd_attr;
    res = pthread_attr_init(&thrd_attr);

    for ( int i = 0; i < READERS; i++) {
        int pid = pthread_create(&thread_array[i].thread_id,
                                    NULL,
                                    &reader_thread,
                                    test_vector);
    }

    for ( int i = READERS; i < THREADS; i++) {
        int pid = pthread_create(&thread_array[i].thread_id,
                                    NULL,
                                    &writer_thread,
                                    test_vector);
    }

    while ( 1 ) {
        printf("Main thread...\n");
        sleep(10);
    }

}
