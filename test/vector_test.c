#include "../vector.h"
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>


#define TEST_VECTOR_LEN  ( 20 )

int array_in[TEST_VECTOR_LEN];
int array_out[TEST_VECTOR_LEN];

int main(void)
{
    printf("Vector test file\n");

    // Let's crate a vector
    struct vector *v; 
    if ( ERROR == vector_construct(&v, sizeof(int)) )
    {
        printf("Failed at constructing a vector\n");
    }

    // Let's put items into the vector
    for( int i = 0; i < TEST_VECTOR_LEN; i++)
    {
        int k = rand();
        array_in[i] = k;
        printf("Put in item: %d\n", k);
        if ( ERROR == vector_put(v, &k) )
        {
            printf("Failed at putting in an item\n");
        }
    }

    // Let's get items out of the vector
    for( int i = 0; i < TEST_VECTOR_LEN; i++)
    {
        int k;
        vector_get(v, i ,&k);
        array_out[i] = k;
        printf("Got out item: %d\n", k);
    }
    
    for( int i = 0; i < TEST_VECTOR_LEN; i++)
    {
        assert(array_in[i] == array_out[i]);
    }

    vector_destroy(v);
}