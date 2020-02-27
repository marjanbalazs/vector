#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <pthread.h>
#include <stdio.h>

#include "../vector.h"

#define ERROR               ( 1 )
#define SUCCESS             ( 0 )

#define INITIAL_CAP         ( 8 )
#define GROWTH_TRIGGER      ( 3/4 )
#define GROWTH_FACTOR       ( 2 )
#define SHRINK_FACTOR       ( 1/2 )

#ifdef MULTITHREADED

// based on W Richard Stevens - Unix Network Programming Volume 2.

struct rwlock {
    int refcount;
    int read_wait_count;
    int write_wait_count;
    pthread_mutex_t mutex;
    pthread_cond_t r_cond;
    pthread_cond_t w_cond;
};

int init_rwlockl(struct rwlock *lock)
{
    lock->refcount = 0;
    lock->read_wait_count = 0;
    lock->write_wait_count = 0;
    pthread_mutex_init(&lock->mutex, NULL);
    pthread_cond_init(&lock->r_cond, NULL);
    pthread_cond_init(&lock->w_cond, NULL);

    return SUCCESS;
}

void rwlock_read_lock(struct rwlock *lock)
{
    pthread_mutex_lock(&lock->mutex);

    // If a writer is currently active or writers are waiting
    while ( lock->refcount < 0 || lock->write_wait_count > 0 ) {
        lock->read_wait_count++;
        pthread_cond_wait(&lock->r_cond,
                    &lock->mutex);
        lock->read_wait_count--;
    }

    lock->refcount++;

    pthread_mutex_unlock(&lock->mutex);
}
 
void rwlock_write_lock(struct rwlock *lock)
{
    pthread_mutex_lock(&lock->mutex);

    if ( lock->refcount != 0 ) {
        lock->write_wait_count++;
        pthread_cond_wait(&lock->w_cond,
                         &lock->mutex);

        lock->write_wait_count--;
    }

    lock->refcount = -1;

    pthread_mutex_unlock(&lock->mutex);
}

void rwlock_unlock(struct rwlock *lock)
{
    pthread_mutex_lock(&lock->mutex);

    if ( lock->refcount > 0) {
        lock->refcount--;
        
    } else if ( lock->refcount = -1 ) {
        lock->refcount = 0;
    } else {}

    if ( lock->write_wait_count > 0 ) {
        if ( lock->refcount == 0 ) {
            pthread_cond_signal(&lock->w_cond);
        }
    } else if ( lock->read_wait_count > 0 ) {
        pthread_cond_broadcast(&lock->r_cond);
    }

    pthread_mutex_unlock(&lock->mutex);
}

#endif

struct vector {
    void* vector;
    size_t size;
    size_t cap;
    size_t elem_size;
#ifdef MULTITHREADED
    struct rwlock lock;
#endif
};

static int vector_grow(struct vector *vector);
static int vector_shrink(struct vector *vector);

int vector_construct(struct vector **vector, size_t elem_size)
{
    int ret = SUCCESS;

    *vector = malloc(sizeof(struct vector));
    if ( vector == NULL ) {
        ret = ERROR;
        goto failed_struct;
    }
    (*vector)->cap = INITIAL_CAP;
    (*vector)->elem_size = elem_size;
    (*vector)->size = 0;

#ifdef MULTITHREADED
    ret = init_rwlockl(&(*vector)->lock);
#endif

    (*vector)->vector = malloc( elem_size * INITIAL_CAP);
    if ( (*vector)->vector == NULL ){
        ret = ERROR;
        goto failed_data;
    }

    return ret;

failed_struct:
    free(vector);
    return ret;

failed_data:
    free((*vector)->vector);
    free(vector);
    return ret;
}

int vector_destroy(struct vector *vector)
{
    free(vector->vector);
    free(vector);

    return SUCCESS;
}

int vector_get(struct vector *vector, size_t num, void *out)
{
    int ret = SUCCESS;

#ifdef MULTITHREADED
    rwlock_read_lock(&(vector->lock));
#endif

    if ( num > vector->size ) {
        ret = ERROR;
    } else {
        char * ptr = vector->vector;
        ptr += num * vector->elem_size;
        memcpy(out, (void*)ptr, vector->elem_size);
    }

#ifdef MULTITHREADED
    rwlock_unlock(&vector->lock);
#endif

    return ret;
}

int vector_put(struct vector *vector, void *in)
{
    int ret = SUCCESS;

#ifdef MULTITHREADED
    printf("Trying to lock...\n");
    rwlock_write_lock(&vector->lock);
    printf("Locked...\n");
#endif

    if (vector->cap > vector->size) {
        char *ptr = (char*)vector->vector;
        memcpy((void*)&ptr[vector->size * vector->elem_size],
                in,
                vector->elem_size);
        vector->size++;
    } else {
        vector_grow(vector);
        vector_put(vector, in);
    }

#ifdef MULTITHREADED
    rwlock_unlock(&vector->lock);
#endif

    return ret;
}

int vector_remove(struct vector *vector, size_t num)
{
    int ret;

    if ( num > vector->size ) {
        ret = ERROR;
    } else {
        char *ptr = (char*)vector->vector;
        // If num == size, meaning its the last element is being removed
        // then we are actually pointing one element size over the buffer
        // Assumption is that no copying will be done since size to copy is 0
        memmove((void*)&ptr[vector->elem_size * num],
                (void*)&ptr[vector->elem_size * (num + 1)],
                vector->elem_size * (vector->size - num  - 1));

        vector->size--;

        ret = SUCCESS;
    }

    return ret;
}

int vector_grow(struct vector *vector)
{
    // TODO: Fix return value
    vector->cap = vector->cap * GROWTH_FACTOR;
    void *new_data = malloc(vector->cap * vector->elem_size);
    memcpy(new_data, 
            vector->vector, 
            vector->size * vector->elem_size);
    free(vector->vector);
    vector->vector = new_data;

    return SUCCESS;
}

int vector_shrink(struct vector *vector)
{
    // TODO: Fix return value
    vector->cap = vector->cap * SHRINK_FACTOR;
    void *new_data = malloc(vector->cap * vector->elem_size);
    memcpy(new_data,
            vector->vector,
            vector->size * vector->elem_size);
    free(vector->vector);
    vector->vector = new_data;

    return SUCCESS;
}

size_t vector_get_size(struct vector *vector)
{
    return vector->size;
}

