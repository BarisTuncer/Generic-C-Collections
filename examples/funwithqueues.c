#include <stdio.h>
#include "queue.h"

// run also with valgrind to check that there is no memory leak
int main(){
    queue s = queue_create(QueueStringOps);
    printf("s is of size = %zu\n", s->size(s));
    printf("s = "); s->print(s);
    char *text[5] = {"Hey", "Kids", "Rock", "and", "Roll"};
    for(int i=0; i<5; i++) s->push(s, text[i]);
    printf("s is of size = %zu\n",s->size(s));
    printf("s = "); s->print(s);
    // printf("%s\n", (char *)s->sbuf->vec; //Not allowed!
    // vector v; // not allowed

    s->pop(s);
    printf("after pop: s = "); s->print(s);
    
    queue q = queue_create(QueueStringOps);
    q->assign(q,s); // get a true copy of s
    printf("q = "); q->print(q);
    s->pop(s);
    printf("s = "); s->print(s);
    q->print(q);
    char *temp;
    temp = (char *)q->front(q);
    while(!q->empthy(q)) {
        printf("q = "); q->print(q);
        printf("front of q = %s\n", temp);
        q->pop(q);
        temp = (char *)q->front(q);
    }

    temp = (char *)s->front(s);
    printf("front of s = %s\n", temp);
    queue_destroy(s);
    queue_destroy(q);

    printf("\n##########  size of queue struct ###################\n");
    printf("size of int on this machine, is : %zu bytes\n", sizeof(int));
    printf("size of queue ( = queue_t *) , is : %zu bytes = %.2f ints \n", sizeof(queue), 1.0*sizeof(queue)/sizeof(int));
    printf("size of stack_t is : %zu bytes  = %.2f ints\n", sizeof(struct queue_t), 1.0*sizeof(struct queue_t)/sizeof(int));
}