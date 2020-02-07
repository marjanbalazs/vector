#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <pthread.h>

#define ERROR               ( 1 )
#define SUCCESS             ( 0 )

struct vector;
typedef struct vector Vector;

int vector_construct(struct vector **vector, size_t elem_size);
int vector_destroy(struct vector *vector);
int vector_get(struct vector *vector, size_t num, void *out);
int vector_put(struct vector *vector, void *in);