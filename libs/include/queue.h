#ifndef __QUEUE_H
#define __QUEUE_H

#include <stdbool.h> // for bool
#include <stddef.h>  // for size_t

typedef struct queueContentsOperations {
    /* returns nonzero if *datum1 == *datum2 */
    int (*Equal)(const void *datum1, const void *datum2);
    /* make a copy of datum that will survive changes to original */
    void *(*Copy)(const void *datum);
    /* free a copy */
    void (*Delete)(void *datum);
    /* optional: print */
    void (*Print)(const void *datum);
}QueueContentsOperations;

typedef struct queueBuffer_t *QueueBuffer;

struct queue_t{
    // private members: safe buffer and content operations
    QueueBuffer sbuf;
    QueueContentsOperations ops;
    // public members:
    // element access
    void *(*front)(struct queue_t *);
    void *(*back)(struct queue_t *);
    // capacity
    bool (*empthy)(struct queue_t *);
    size_t (*size)(struct queue_t *v);
    // modifiers
    void (*push)(struct queue_t *, void *data);
    void (*pop)(struct queue_t *);
    void (*swap)(struct queue_t *, size_t i, size_t j);
    // assignment
    void  (*assign)(struct queue_t *v, struct queue_t *w);
    bool  (*equal)(struct queue_t *v, struct queue_t *w);
    // info: provided if QueueContentsOperations.Print is not NULL
    void (*print)(struct queue_t *);
    void (*fprint)(struct queue_t *);
};

typedef struct queue_t *queue;

/* constructor and destructor */
queue queue_create(QueueContentsOperations); /* returns 0 on allocation error */
void queue_destroy(queue);

/* Some predefined StackContentsOperations structures */
extern QueueContentsOperations QueueIntOps;
extern QueueContentsOperations QueueStringOps;

#endif // __QUEUE_H