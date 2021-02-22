#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "vector.h"
#include "queue.h"

struct queueBuffer_t{
    vector vec;
};

static size_t queue_size(queue s){
    vector vec = s->sbuf->vec;
    return vec->size(vec);
}

static bool queue_empthy(queue s){
    vector vec = s->sbuf->vec;
    return vec->empty(vec);
}

static void queue_pop(queue s){
    vector vec = s->sbuf->vec;
    vec->pop_front(vec);
}

static void queue_print(queue s){
    printf("<<");
    vector v = s->sbuf->vec;
    if(s->size(s) < 20){
        for(size_t i = 0; i < v->size(v); i++) {
            printf("[%ld]: ", i);
            s->ops.Print(v->at(v,i));
        }
    }
    else {  
        for(int i = 0; i < 5; i++) {
            printf("[%d]: ", i);
            s->ops.Print(v->at(v,i));
        }
        printf(" ..skipping the middle.. ");
        for(size_t i = v->size(v)-5; i < v->size(v); i++) {
            printf("[%ld]: ", i);
            s->ops.Print(v->at(v,i));
        }      
    }
    printf(">>\n");
}

static void queue_push(queue s, void *data){
    vector vec = s->sbuf->vec;
    vec->push_back(vec, data);
}

static void queue_swap(queue s, size_t i, size_t j){
    vector vec = s->sbuf->vec;
    vec->swap(vec, i, j);
}

static void *queue_back(queue s){
    vector vec = s->sbuf->vec;
    return vec->back(vec);
}

static void *queue_front(queue s){
    vector vec = s->sbuf->vec;
    return vec->front(vec);
}

static void  queue_assign(queue s, queue q){
    vector vs = s->sbuf->vec;
    vector vq = q->sbuf->vec;
    return vs->assign(vs, vq);    
}

static bool  queue_equal(queue s, queue q){
    vector vs = s->sbuf->vec;
    vector vq = q->sbuf->vec;
    return vs->equal(vs, vq);
}

queue queue_create(QueueContentsOperations Ops) {
    queue stk = (queue)malloc(sizeof(struct queue_t));
    if(!stk) return NULL;
    stk->ops = Ops;
    QueueBuffer bf = (QueueBuffer)malloc(sizeof(struct queueBuffer_t));
    stk->sbuf = bf;
    struct vectorContentsOperations vops = {Ops.Equal, Ops.Copy, Ops.Delete, Ops.Print, NULL};
    vector vec = vector_create(vops);
    if(!vec) { free(bf); free(stk); return NULL;}
    stk->sbuf->vec = vec;
    // assign member variables
    stk->front = queue_front;
    stk->back = queue_back;
    stk->pop = queue_pop;
    stk->empthy = queue_empthy;
    stk->push = queue_push;
    stk->swap = queue_swap;
    stk->print = queue_print;
    stk->assign = queue_assign;
    stk->equal = queue_equal;
    stk->size = queue_size;
    return stk;
}

void queue_destroy(queue s){
    vector_destroy(s->sbuf->vec);
    free(s->sbuf);
    free(s);
}


/* int functions */
static int vectorIntEqual(const void *x, const void *y) { return (*(int *) x) == (*(int *) y);}
static void *vectorIntCopy(const void *x) { 
    int *temp = (int *)malloc(sizeof(int));
    *temp = *(int *)x;  
    return  (void *)temp; 
}
static void vectorIntDelete(void *x) { free((int *)x); }
static void vectorIntPrint(const void *x){ printf("%d ", *(int *)x);}

QueueContentsOperations QueueIntOps = {.Equal=vectorIntEqual, .Copy=vectorIntCopy, 
.Delete=vectorIntDelete, .Print=vectorIntPrint};

/* string functions */
static int vectorStringEqual(const void *x, const void *y){ return !strcmp((const char *) x, (const char *) y);}    
static void *vectorStringCopy(const void *x){ return strdup((char *)x);}
static void vectorStringFree(void *x) { free((char *)x); }
static void vectorStringPrint(const void *x){ printf("%s ", (char *)x);}

QueueContentsOperations QueueStringOps = { .Equal = vectorStringEqual, 
.Copy=vectorStringCopy, .Delete=vectorStringFree, .Print=vectorStringPrint};
