#include <stdio.h>
#include "stack.h"

// run also with valgrind to check that there is no memory leak
int main(){
    stack s = stack_create(StackStringOps);
    printf("s is of size = %zu\n", s->size(s));
    printf("s = "); s->print(s);
    char *text[5] = {"Hey", "Kids", "Rock", "and", "Roll"};
    for(int i=0; i<5; i++) s->push(s, text[i]);
    printf("s is of size = %zu\n",s->size(s));
    printf("s = "); s->print(s);
    // printf("%s\n", (char *)s->sbuf->vec->buf[0]); //Not allowed!
    // vector v; // not allowed (if vector.h is not included. Even so, you can't access)
    s->pop(s);
    printf("s = "); s->print(s);
    stack q = stack_create(StackStringOps);
    q->assign(q,s); // get a true copy of s
    printf("q = "); q->print(q);
    s->pop(s);
    printf("s = "); s->print(s);
    printf("q = "); q->print(q);
    char *temp;
    temp = (char *)q->top(q);
    while(!q->empthy(q)) {
        printf("q = "); q->print(q);
        printf("top of q = %s\n", temp);
        q->pop(q);
        temp = (char *)q->top(q);
    }

    temp = (char *)s->top(s);
    printf("top of s = %s\n", temp);
    stack_destroy(s);
    stack_destroy(q);

    printf("\n##########  size of stack struct ###################\n");
    printf("size of bool on this machine, is : %zu bytes\n", sizeof(bool));
    printf("size of int on this machine, is : %zu bytes\n", sizeof(int));
    printf("size of stack ( = stack_t *) , is : %zu bytes = %.2f ints \n", sizeof(stack), 1.0*sizeof(stack)/sizeof(int));
    printf("size of stack_t is : %zu bytes  = %.2f ints\n", sizeof(struct stack_t), 1.0*sizeof(struct stack_t)/sizeof(int));
}