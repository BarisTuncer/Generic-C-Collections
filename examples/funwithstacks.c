#include <stdio.h>
#include "stack.h"

int main(){
    stack s = stack_create(StackStringOps);
    printf("s is of size = %zu\n", s->size(s));
    printf("s = "); s->print(s);
    char *text[5] = {"Hey", "Kids", "Rock", "and", "Roll"};
    for(int i=0; i<5; i++) s->push(s, text[i]);
    printf("s is of size = %zu\n",s->size(s));
    printf("s = "); s->print(s);
    // printf("%s\n", (char *)s->sbuf->vec->buf[0]); //Not allowed!
    // vector v; // not allowed

    s->pop(s);
    s->print(s);
    stack q = stack_create(StackStringOps);
    q->assign(q,s); // get a true copy of s
    q->print(q);
    s->pop(s);
    q->print(s);
    q->print(q);
    while(!q->empthy(q)) {
        printf("q = "); q->print(q);
        q->pop(q);
    }

    char *temp = (char *)s->top(s);
    printf("top of s = %s\n", temp);
    stack_destroy(s);
    stack_destroy(q);
}