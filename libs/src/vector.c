#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include "vector.h"

#define VECTOR_MIN_CAP (2)

struct vectorBuffer_t{
    void **arr;
    size_t capacity;
    size_t size;
};

static void vector_error(vector v, const char *funcname, char *msg){
    if(v) v->print(v);
    else printf("vector is NULL\n");
    int errnum = errno;
    fprintf(stderr, "Upps! in %s %s: %s\n", funcname,  msg, strerror( errnum ));
    abort();
}

static void check_range(vector v, size_t ind){
    if(ind>=v->buffer->size){
        errno = EFAULT;
        vector_error(v, __func__, "index out of bound");
    }
    return;
}

static size_t vector_size(vector v){
    return v->buffer->size;
}

static size_t vector_capacity(vector v){
    return v->buffer->capacity;
}

static void vector_print(vector v){
    if(!v->ops.Print) return;
    printf("<<");
    if(v->buffer->size < 20){
        for(size_t i = 0; i < v->buffer->size; i++) {
            printf("[%ld]: ", i);
            v->ops.Print(v->buffer->arr[i]);
        }
    }
    else {  
        for(int i = 0; i < 5; i++) {
            printf("[%d]: ", i);
            v->ops.Print(v->buffer->arr[i]);            
        }
        printf(" ..skipping the middle.. ");
        for(size_t i = v->buffer->size-5; i < v->buffer->size; i++) {
            printf("[%ld]: ", i);
            v->ops.Print(v->buffer->arr[i]);            
        }       
    }
    printf(">>\n");
}

static void vector_pushback(vector vec, void *value){
    if(vec->buffer->size == vec->buffer->capacity) { 
        // printf("reallocating\n");
        void **temp = (void **)malloc(sizeof(void *) * 2*vec->buffer->capacity);
        if(!temp) abort();
        // clear new table
        for(size_t i=0; i<2*vec->buffer->capacity; i++) temp[i] = NULL;
        // move all elements of old table to new table
        for(size_t i=0; i<vec->buffer->capacity; i++) temp[i] = vec->buffer->arr[i];
        free(vec->buffer->arr);
        vec->buffer->arr = temp;
        vec->buffer->capacity = 2*vec->buffer->capacity;
    }
    vec->buffer->arr[vec->buffer->size++] = vec->ops.Copy(value);
}

static void *vector_at(vector v, size_t ind){
    if(!v) return NULL;
    check_range(v, ind);
    return v->buffer->arr[ind];
}

static void *vector_front(vector v){
    if(!v || v->buffer->size == 0) return NULL;
    return v->buffer->arr[0];
}

static void *vector_back(vector v){
    if(!v || v->buffer->size == 0) return NULL;
    return v->buffer->arr[v->buffer->size-1];
}

static void vector_get(vector v, size_t ind, void **data){
    if(!v) return;
    check_range(v, ind);
    if(*data) v->ops.Delete(*data);
    *data = v->ops.Copy(v->buffer->arr[ind]);
}

static void vector_set(vector v, size_t ind, void *value){
    check_range(v, ind);
    v->ops.Delete(v->buffer->arr[ind]); 
    v->buffer->arr[ind] = v->ops.Copy(value);
}

static void vector_clear(vector v){
    for(size_t i=0; i<v->buffer->size; i++)
        v->ops.Delete(v->buffer->arr[i]);
    v->buffer->size = 0;
}

static void vector_swap(vector v, size_t i, size_t j){
    check_range(v, i);
    check_range(v, j);
    if (v->ops.Equal(v->buffer->arr[i], v->buffer->arr[j])) return;
    void *temp = v->buffer->arr[i];
    v->buffer->arr[i] = v->buffer->arr[j];
    v->buffer->arr[j] = temp;
}

static bool vector_empty(vector v){
    return v->buffer->size == 0;
}

static void vector_insert(vector v, size_t ind, void *data){
    check_range(v, ind); 
    vector_pushback(v, data); // we need one more spot, this may require re-sizing!
    v->ops.Delete(v->back(v));   // clean the last spot
    for(size_t i=v->buffer->size-1;  i>ind;  i--){
        v->buffer->arr[i] = v->buffer->arr[i-1];        
    }
    v->buffer->arr[ind] = v->ops.Copy(data);
    return;
}

static void vector_popback(vector v){
    if(v->buffer->size == 0) return;
    v->ops.Delete(v->back(v));   // clean the last spot
    v->buffer->size--;
}

static void vector_popfront(vector v){
    if(v->buffer->size == 0) return;
    v->ops.Delete(v->front(v));   // clean the first spot
    v->buffer->size--;
}

static void vector_erase(vector v, size_t ind, size_t len){
    check_range(v, ind);
    if(ind + len > v->buffer->size) vector_error(v, __func__, " ind + len > vector size");
    for(size_t i=ind;  i<ind + len;  i++){
        v->ops.Delete(v->buffer->arr[i]);
    }
    for(size_t i=ind+len; i<v->buffer->size; i++){
            v->buffer->arr[i-len] = v->buffer->arr[i];
    }
    for(size_t i=0; i<len; i++) v->buffer->arr[v->buffer->size-1-i] = NULL;
    v->buffer->size -= len;
    return;
}

static bool vector_equal(vector v, vector w){
    if(v->buffer->size != w->buffer->size) return false;
    for(size_t i=0; i<v->buffer->size; i++){
        if(!v->ops.Equal(v->buffer->arr[i], w->buffer->arr[i])){
            return false;
        }
    }
    return true;
}

static void vector_assign(vector v, vector w){
    if(v->buffer->size > 0){
        for(size_t i=0;  i<v->buffer->size;  i++){
            v->ops.Delete(v->buffer->arr[i]);
        }
        v->buffer->size = 0;
    }
    for(size_t i=0; i<w->buffer->size; i++) v->push_back(v, w->at(w,i));
}

static int vector_find(vector v, void *data){
    for(size_t i=0; i<v->buffer->size; i++){
        if(v->ops.Equal(v->buffer->arr[i], data)){
            return i;
        }
    }
    return -1;
}

static void vector_sort(vector v){
    if(v->buffer->size < 2) return;
    if(v->ops.Compare){ 
        qsort(v->buffer->arr, v->buffer->size, sizeof(void *), v->ops.Compare);
    }
}

static void vector_pushfront(vector v, void *data){
    vector_insert(v, 0, data);
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
static int vectorIntCompare(const void *x, const void *y){return(*(const int **)x - *(const int **)y);}

VectorContentsOperations VectorIntOps = {.Equal=vectorIntEqual, .Copy=vectorIntCopy, 
.Delete=vectorIntDelete, .Print=vectorIntPrint, .Compare=vectorIntCompare };

/* string functions */
static int vectorStringEqual(const void *x, const void *y){ return !strcmp((const char *) x, (const char *) y);}    
static void *vectorStringCopy(const void *x){ return strdup((char *)x);}
static void vectorStringFree(void *x) { free((char *)x); }
static void vectorStringPrint(const void *x){ printf("%s ", (char *)x);}
static int vectorStringCompare(const void *x, const void *y){ return(strcmp(*(const char **)x, *(const char **)y));}

VectorContentsOperations VectorStringOps = { .Equal = vectorStringEqual, 
.Copy=vectorStringCopy, .Delete=vectorStringFree, .Print=vectorStringPrint, .Compare=vectorStringCompare};

vector vector_create(VectorContentsOperations Ops) {
    vector vec = (vector)malloc(sizeof(struct vector_t));
    if(!vec) return NULL;
    VectorBuffer vecbuf = (VectorBuffer)malloc(sizeof(struct vectorBuffer_t));
    if(!vecbuf) return NULL;
    vec->buffer = vecbuf;
    vec->buffer->size = 0;
    vec->buffer->capacity = VECTOR_MIN_CAP;
    vec->buffer->arr = (void **)malloc(sizeof(void *) * vec->buffer->capacity);
    if(!vec->buffer->arr){
        free(vec);
        return NULL;
    }
    for(size_t i=0; i<vec->buffer->capacity; i++) vec->buffer->arr[i] = NULL;
    vec->ops = Ops;
    // assign member variables
    vec->push_back = vector_pushback;
    vec->push_front = vector_pushfront;
    vec->pop_front = vector_popfront;
    vec->pop_back = vector_popback;
    vec->print = vector_print;
    vec->at = vector_at;
    vec->get = vector_get;
    vec->front = vector_front;
    vec->back = vector_back;
    vec->set = vector_set;
    vec->clear = vector_clear;
    vec->swap = vector_swap;
    vec->insert = vector_insert;
    vec->find = vector_find;
    vec->empty = vector_empty;
    vec->assign = vector_assign;
    vec->erase = vector_erase;
    vec->equal = vector_equal;
    vec->size = vector_size;
    vec->capacity = vector_capacity;
    vec->sort = vector_sort;
    return vec;
}

void vector_destroy(vector v){
    for(size_t i= 0; i < v->buffer->size; i++) {
        v->ops.Delete(v->buffer->arr[i]);
    }
    free(v->buffer->arr);
    free(v->buffer);
    free(v);
}
