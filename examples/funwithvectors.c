#include <stdio.h>
#include <stdlib.h>  // for rand
#include <time.h>    // for timing
#include <string.h>
#include "vector.h"


static vector *vclean;
static size_t nclean;

void clean(){
    for(size_t i=0; i<nclean; i++) vector_destroy(vclean[i]);
    free(vclean);
}

vector Vector_create(VectorContentsOperations vop){
    vector v = vector_create(vop);
    vclean = realloc(vclean, sizeof(vector)* (nclean + 1));
    vclean[nclean++] = v;
    return v;
}


// pass by reference example which is usually the case
void removeConsecutiveDuplicatesInPlace(vector v){
    if(v->size(v) == 1) return;
    for(size_t i=0; i<v->size(v)-1; i++){
        while(v->ops.Equal(v->at(v,i), v->at(v, i+1))){
            v->erase(v, i+1, 1);
            i--;
        }
    }
}

// in case you need pass by value example
vector removeConsecutiveDuplicates(vector v, VectorContentsOperations vops){
    vector w = Vector_create(vops); // need to get a copy, not to modify the argument vector
    w->assign(w,v);
    if(w->size(v) == 1) return w;
    for(size_t i=0; i<w->size(w)-1; i++){
        while(w->ops.Equal(w->at(w,i), v->at(w, i+1))){
            w->erase(w, i+1, 1);
            i--;
        }
    }
    return w;
}

/* Exp: Employee functions */

struct employee_t{ char *name; int salary; };
typedef struct employee_t *employee;

static int vectorEmployeeEqual(const void *x, const void *y){
    employee first = (employee)x;
    employee second = (employee)y;
    int salaries = (first->salary) == (second->salary);
    int names = !strcmp(first->name, second->name);
    return salaries && names;
}

static void *vectorEmployeeCopy(const void *x){ 
    employee emp = (employee)malloc(sizeof(struct employee_t));
    emp->name = strdup(((employee)x)->name);
    emp->salary = ((employee)x)->salary;      
    return (void *)emp;
}

static void vectorEmployeeFree(void *x) {
    employee emp = (employee)x;
    free(emp->name);
    free(emp);
}

// optional if printing needed, else set to NULL
static void vectorEmployeePrint(const void *x){ 
    employee emp = (employee)x;
    printf("(name:%s, salary:%d) ", emp->name, emp->salary);
}

// optional if sorting needed else set to NULL
static int vectorEmployeeCompare(const void *x, const void *y){ 
    employee emp1 = (employee)x;
    employee emp2 = (employee)y;
    return strcmp(*(char **)emp1->name, *(char **)emp2->name);
}

VectorContentsOperations VectorEmployeeOps = { vectorEmployeeEqual, vectorEmployeeCopy, vectorEmployeeFree, vectorEmployeePrint, vectorEmployeeCompare};


/* Exp: Vector of vectors */

static int vectorVectorEqual(const void *x, const void *y){
    vector v = (vector)x;
    vector w = (vector)y;
    return v->equal(v,w);
}

static void *vectorVectorCopy(const void *x){ 
    vector v = (vector)x;
    vector w = vector_create(v->ops);
    w->assign(w,v);
    return (void *)w;
}

static void vectorVectorFree(void *x) {
    vector v = (vector)x;
    vector_destroy(v);
}

// optional if printing needed, else set to NULL
static void vectorVectorPrint(const void *x){ 
    vector v = (vector)x;
    v->print(v);
}

VectorContentsOperations VectorVectorOps = { vectorVectorEqual, vectorVectorCopy, vectorVectorFree, vectorVectorPrint, NULL};


// also run with Valgrind 
int main(){
    // register clean to keep the track of created vectors and cleanup before the exit
    // so we don't need to remember to call vector_destroy
    atexit(clean);   
    // ATTENTION: we wrapped vector_create with Vector_create for cleanup, see its definition above
    vector v = Vector_create(VectorStringOps);
    vector v3 = Vector_create(VectorStringOps);
    printf("v = "); v->print(v);
    char *a = "Baris";
    char *b = "Tuncer"; 
    char *c = "Hello";
    char *d = "World";
    char *e = "Rock";
    v->push_back(v , a);
    v->push_back(v , b);
    v->push_back(v , c);
    v->push_back(v , d);
    v->push_back(v , e);

    // update values: this should not have an effect on vector contents
    a = b;
    printf("v = "); v->print(v);

    printf("v[3] = %s\n", (char *)v->at(v,3));

    printf("setting v[2] = SetTest\n");

    //v->buffer->size = 10; // not allowed!

    char *SetTest = "setTest";
    v->set(v, 2, SetTest);
    printf("v = "); v->print(v);

    printf("swapping index 2 and 4\n");

    v->swap(v, 2, 4);
    printf("v = "); v->print(v);

    printf("inserting at v[2] = insertat2Test\n");

    char *Insertat2Test = "Insertat2Test";
    v->insert(v, 2, Insertat2Test);
    printf("v = "); v->print(v);

    char *InsertatLastTest = "InsertatLastTest";
    v->insert(v, v->size(v)-1, InsertatLastTest);
    printf("v = "); v->print(v);

    char *InsertFirstTest = "InsertFirstTest";
    v->insert(v, 0, InsertFirstTest);
    printf("v = "); v->print(v);

    printf("assign v to v3 : v3 = v1\n");
    v3->assign(v3, v);
    printf("v3 = "); v3->print(v3);

    printf("Sorting v3\n");
    v3->sort(v3);
    printf("v3 = "); v3->print(v3);
    printf("v shouldn't have changed\n");
    printf("v = "); v->print(v);

    printf("Vector of vectors!\n");
    vector vov = Vector_create(VectorVectorOps);
    vov->push_back(vov,(void *)v);
    vov->push_back(vov,(void *)v3);
    printf("vov = "); vov->print(vov);
    vector tempvec = (vector)vov->at(vov,0);
    char *forvov = "Olala";
    tempvec->push_back(tempvec,forvov);
    printf("vov = "); vov->print(vov);

    printf("Erasing 2 elements starting from index 1\n");
    v->erase(v,1,2);
    printf("v = "); v->print(v);

    printf("Pop back!\n");
    v->pop_back(v);
    printf("v = "); v->print(v);

    printf("Erasing 2 elements starting from index 3\n");
    v->erase(v,3,2);
    printf("v = "); v->print(v);


    printf("searching for %s\n", e);
    int ret = v->find(v, e);
    if(ret != -1) printf("%s found at index %d\n", e, ret);
    else printf("%s couldn't be found\n", e);

    printf("searching for %s\n", a);
    ret = v->find(v, a);
    if(ret != -1) printf("%s found at index %d\n", a, ret);
    else printf("%s couldn't be found\n", a);

    vector v2 = Vector_create(VectorStringOps);
    char *a2 = "C";
    char *b2 = "is"; 
    char *c2 = "alive";
    v2->push_back(v2 , a2);
    v2->push_back(v2 , b2);
    v2->push_back(v2 , c2);
    printf("v2 = "); v2->print(v2);

    printf("Assing v2 to v: v = v2\n");
    v->assign(v, v2);
    printf("v = "); v->print(v);
    printf("v2 = "); v2->print(v2);

    printf("confirm that they are equal: v = v2\n");
    if(v->equal(v, v2)) printf("confirmed!\n");
    else printf("not confirmed!\n");

    printf("lets get teststr = v[2]\n");
    char *tempbuf = malloc(10);
    v->get(v,2, (void **)&tempbuf); // copy the content of v[2] into tempbuf
    printf("test = %s\n", (char *)tempbuf);
    printf("modify test\n");
    tempbuf[1] = 'L';
    printf("test = %s\n", tempbuf);
    printf("what about v?\n");
    printf("v = "); v->print(v);

    printf("These guys should be alive (except a since we set it to point b ) and un-effected: %s %s %s %s %s \n", a, b, c, d, e);
    printf("These guys should be alive and un-effected: %s %s %s\n", a2, b2, c2);


    printf("\n##########   int vectors ###################\n");
    vector w = Vector_create(VectorIntOps);
    time_t t;
    srand((unsigned) time(&t));

    for(int i=0; i<10; i++){
        int temp = rand() % 50;
        w->push_back(w, &temp);
    }
    printf("w = "); w->print(w);

    printf("\n########## sorting ###################\n");
    printf("sorting starts \n");
    w->sort(w);
    printf("sorting ends \n");
    printf("w = "); w->print(w);

    printf("\n########## setting some entries ###################\n");
    int i = 10;
    w->set(w, 2, &i);
    w->set(w, 3, &i);
    w->set(w, 4, &i);

    printf("w = "); w->print(w);
    printf("passing into a function!\n");
    printf("removeConsecutiveDuplicates\n");
    printf("Not in place!\n");
    vector w2 = removeConsecutiveDuplicates(w, VectorIntOps);
    printf("w = "); w->print(w);
    printf("w2 = "); w2->print(w2); 

    printf("In place!\n");
    removeConsecutiveDuplicatesInPlace(w);
    printf("w = "); w2->print(w);

    printf("Vector of vectors!\n");
    vector vv = Vector_create(VectorVectorOps);
    vv->push_back(vv,w);
    vv->push_back(vv,w2);
    printf("vv = "); vv->print(vv);
    vector w3 = (vector)vv->at(vv,0);
    i = 9999;
    w3->push_back(w3,&i);
    printf("vv = "); vv->print(vv);

    free(tempbuf);
    printf("\n##########   employee vectors ###################\n");
    employee emp1 = (employee)malloc(sizeof(struct employee_t));
    employee emp2 = (employee)malloc(sizeof(struct employee_t));
    emp1->name = "Bob";  emp1->salary= 60000;
    emp2->name = "Mike"; emp2->salary= 80000;
    vector empvec = Vector_create(VectorEmployeeOps);
    empvec->push_back(empvec, (void *)emp2);
    empvec->push_back(empvec, (void *)emp1);
    empvec->print(empvec);
    empvec->sort(empvec); 
    printf("after sort:"); empvec->print(empvec);
    empvec->pop_back(empvec);
    empvec->print(empvec);
    employee temp = (employee)empvec->at(empvec, 0);
    printf("empvec[0] = (name: %s, salary:%d)\n", temp->name, temp->salary);
    empvec->push_front(empvec, (void *)emp2);
    empvec->print(empvec);
    empvec->pop_back(empvec);
    empvec->pop_back(empvec);
    empvec->print(empvec);
    printf("emp1 should be alive: %s %d\n", emp1->name, emp1->salary);
    printf("emp2 should be alive: %s %d\n", emp2->name, emp2->salary);
    free(emp1);
    free(emp2);

    empvec->at(empvec, 11);

    printf("\n##########  size of vector struct ###################\n");
    printf("size of int on this machine, is : %zu bytes\n", sizeof(int));
    printf("size of vector ( = vector_t *) , is : %zu bytes = %.2f ints \n", sizeof(vector), 1.0*sizeof(vector)/sizeof(int));
    printf("size of vector_t is : %zu bytes  = %.2f ints\n", sizeof(struct vector_t), 1.0*sizeof(struct vector_t)/sizeof(int));

    return 0;
}