#ifndef __VECTOR_H_
#define __VECTOR_H_

#include <stddef.h>  // for size_t
#include <stdbool.h> // for bool

/* Provides operations for working with entries */
typedef struct vectorContentsOperations {
    /* returns nonzero if *datum1 == *datum2 */
    int (*Equal)(const void *datum1, const void *datum2);
    /* make a copy of datum that will survive changes to original */
    void *(*Copy)(const void *datum);
    /* free a copy */
    void (*Delete)(void *datum);
    /* optional: print */
    void (*Print)(const void *datum);
    /* optional: for sorting */
    int (*Compare)(const void *datum1, const void *datum2);
}VectorContentsOperations;

/* Some predefined vectorContentsOperations structures */
// make them available for clients


extern VectorContentsOperations VectorIntOps;
extern VectorContentsOperations VectorStringOps;

typedef struct vectorBuffer_t *VectorBuffer;

struct vector_t {
    VectorBuffer buffer;
    VectorContentsOperations ops;
    // member variables
    size_t (*size)(struct vector_t *v);
    size_t (*capacity)(struct vector_t *v);
    void (*push_back)(struct vector_t *v, void *data);
    void (*push_front)(struct vector_t *v, void *data);
    void (*pop_front)(struct vector_t *v);
    void (*pop_back)(struct vector_t *v);
    void (*print)(struct vector_t *v);
    void *(*at)(struct vector_t *v, size_t ind);
    void (*get)(struct vector_t *v, size_t ind, void **data);
    void *(*front)(struct vector_t *v);
    void *(*back)(struct vector_t *v);
    void  (*set)(struct vector_t *v, size_t ind, void *data);
    void  (*clear)(struct vector_t *v);
    void  (*swap)(struct vector_t *v, size_t ind1, size_t ind2);
    void  (*insert)(struct vector_t *v, size_t ind, void *data);
    void  (*erase)(struct vector_t *v, size_t startind, size_t len);
    int   (*find)(struct vector_t *v, void *data);
    bool  (*empty)(struct vector_t *v);
    void  (*assign)(struct vector_t *v, struct vector_t *w);
    bool  (*equal)(struct vector_t *v, struct vector_t *w);
    void  (*sort)(struct vector_t *v);
};

typedef struct vector_t *vector;

/* create a new vector with given operations */
vector vector_create(VectorContentsOperations ops);
/* destroy the created vector */
void vector_destroy(vector v);

#endif //__VECTOR_H_