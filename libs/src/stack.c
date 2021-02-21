#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "vector.h"
#include "stack.h"

struct stackBuffer_t{
    vector vec;
};

static size_t stack_size(stack s){
    vector vec = s->sbuf->vec;
    return vec->size(vec);
}

static bool stack_empthy(stack s){
    vector vec = s->sbuf->vec;
    return vec->empty(vec);
}

static void stack_pop(stack s){
    vector vec = s->sbuf->vec;
    vec->pop_back(vec);
}

static void stack_print(stack s){
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

static void stack_push(stack s, void *data){
    vector vec = s->sbuf->vec;
    vec->push_back(vec, data);
}


static void stack_swap(stack s, size_t i, size_t j){
    vector vec = s->sbuf->vec;
    vec->swap(vec, i, j);
}

static void *stack_top(stack s){
    vector vec = s->sbuf->vec;
    return vec->back(vec);
}

static void  stack_assign(stack s, stack q){
    vector vs = s->sbuf->vec;
    vector vq = q->sbuf->vec;
    return vs->assign(vs, vq);    
}

static bool  stack_equal(stack s, stack q){
    vector vs = s->sbuf->vec;
    vector vq = q->sbuf->vec;
    return vs->equal(vs, vq);
}

stack stack_create(StackContentsOperations Ops) {
    stack stk = (stack)malloc(sizeof(struct stack_t));
    if(!stk) return NULL;
    stk->ops = Ops;
    StackBuffer bf = (StackBuffer)malloc(sizeof(struct stackBuffer_t));
    stk->sbuf = bf;
    struct vectorContentsOperations vops = {Ops.Equal, Ops.Copy, Ops.Delete, Ops.Print, NULL};
    vector vec = vector_create(vops);
    if(!vec) { free(bf); free(stk); return NULL;}
    stk->sbuf->vec = vec;
    // assign member variables
    stk->pop = stack_pop;
    stk->empthy = stack_empthy;
    stk->push = stack_push;
    stk->swap = stack_swap;
    stk->top = stack_top;
    stk->print = stack_print;
    stk->assign = stack_assign;
    stk->equal = stack_equal;
    stk->size = stack_size;
    return stk;
}

void stack_destroy(stack s){
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

StackContentsOperations StackIntOps = {.Equal=vectorIntEqual, .Copy=vectorIntCopy, 
.Delete=vectorIntDelete, .Print=vectorIntPrint};

/* string functions */
static int vectorStringEqual(const void *x, const void *y){ return !strcmp((const char *) x, (const char *) y);}    
static void *vectorStringCopy(const void *x){ return strdup((char *)x);}
static void vectorStringFree(void *x) { free((char *)x); }
static void vectorStringPrint(const void *x){ printf("%s ", (char *)x);}

StackContentsOperations StackStringOps = { .Equal = vectorStringEqual, 
.Copy=vectorStringCopy, .Delete=vectorStringFree, .Print=vectorStringPrint};