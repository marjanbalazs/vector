 
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

#define READERS     ( 10 )
#define WRITERS     ( 2 )
#define THREADS     ( READERS + WRITERS )

struct thread_info thread_array[THREADS];


void* reader_thread(void *arg)
{
    while (1) {
        printf("Reader thread pid: %d\n", pthread_self());
        sleep(1);
    }
}

void* writer_thread(void *arg)
{
    while (1) {
        printf("Writer thread pid: %d\n", pthread_self());
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
