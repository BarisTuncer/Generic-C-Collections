#ifndef __STACK_H
#define __STACK_H

#include <stdbool.h> // for bool
#include <stddef.h>  // for size_t

typedef struct stackContentsOperations {
    /* returns nonzero if *datum1 == *datum2 */
    int (*Equal)(const void *datum1, const void *datum2);
    /* make a copy of datum that will survive changes to original */
    void *(*Copy)(const void *datum);
    /* free a copy */
    void (*Delete)(void *datum);
    /* optional: print */
    void (*Print)(const void *datum);
}StackContentsOperations;

typedef struct stackBuffer_t *StackBuffer;

struct stack_t{
    StackBuffer sbuf;
    StackContentsOperations ops;
    // element access
    void *(*top)(struct stack_t *);
    // capactiy
    bool (*empthy)(struct stack_t *);
    size_t (*size)(struct stack_t *v);
    // modifiers
    void (*push)(struct stack_t *, void *data);
    void (*pop)(struct stack_t *);
    void (*swap)(struct stack_t *, size_t i, size_t j);
    // assignment
    void  (*assign)(struct stack_t *v, struct stack_t *w);
    bool  (*equal)(struct stack_t *v, struct stack_t *w);
    // info: provided if StackContentsOperations.Print is not NULL
    void (*print)(struct stack_t *);
};

typedef struct stack_t *stack;

/* constructor and destructor */
stack stack_create(StackContentsOperations); /* returns 0 on allocation error */
void stack_destroy(stack);

/* Some predefined StackContentsOperations structures */
extern StackContentsOperations StackIntOps;
extern StackContentsOperations StackStringOps;

#endif // __STACK_H