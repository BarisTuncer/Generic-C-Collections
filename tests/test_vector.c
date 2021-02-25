#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <stdlib.h>
#include <stdint.h>
#include <cmocka.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <signal.h>
#include <errno.h>
#include <sys/wait.h>
#include <unistd.h>
#include "vector.h"

#define static
#define UNUSED(x) (void)(x)
#define STRESS_TEST_ITERATIONS (1000)


/* Declare the variables your tests want to use. */

static vector v;
static vector w;
static vector sv;
static vector sw;
static int i;
static volatile int sr;

void __wrap_exit(int status);
static sigjmp_buf env;
static void Signal(int signum, void *sighandler){
    struct sigaction newhandler;                        /* new settings        */
    newhandler.sa_handler = sighandler;                 /* handler function    */
	newhandler.sa_flags = SA_RESETHAND;    /* options    */
	if(sigaction(signum, &newhandler, NULL) == -1 )
		printf("Oh dear, something went wrong with sigaction! %s\n", strerror(errno));
}

static void sigusr1_handler(int signo){
    printf("Received SIGCHLD: %d\n", signo);
    assert_true(true);
    siglongjmp(env, 1);  // longjmp() ``returns'' to the state of the program when setjmp() was called.
    // so it will set the return of setjmp (which is i) to 1 --> which will cause return
}

/** These functions will be used to initialize
   and clean resources up after each test run 
*/

int setup (void **state){
    UNUSED(state);
    v = vector_create(VectorIntOps); 
    w = vector_create(VectorIntOps);
    sv = vector_create(VectorStringOps); 
    sw = vector_create(VectorStringOps);
    i = 0;
    assert_non_null(v);
    assert_non_null(w);
    return 0;
}

int teardown (void **state){
    UNUSED(state);
    vector_destroy(v);
    vector_destroy(w);
    vector_destroy(sv);
    vector_destroy(sw);
    return 0;
}


/**
 * assert_* versions generate fatal failures when they fail, and abort the current function. 
 */

void test1(){
    assert_non_null(v);
    i = 1;
    v->push_back(v, &i);
    i = 2;
    v->push_back(v, &i);
    i = 3;
    v->push_back(v, &i);
    assert_false(v->empty(v));
    assert_int_equal(*(int *)v->back(v), 3);
    v->pop_back(v);
    assert_false(v->empty(v));
    assert_int_equal(*(int *)v->back(v), 2);
    v->pop_back(v);
    assert_false(v->empty(v));    
    assert_int_equal(*(int *)v->back(v), 1);
    v->pop_back(v);
    assert_true(v->empty(v));
}

void test2(){
    // let's do some stress testing
    for(i = 0; i < STRESS_TEST_ITERATIONS; i++) {
        v->push_back(v, &i);
    }
    int j;
    for(j = STRESS_TEST_ITERATIONS - 1; j >= 0; j--) {
        assert_false(v->empty(v));
        assert_int_equal(*(int *)v->back(v), j);
        v->pop_back(v);
    }      
    assert_true(v->empty(v));
}

void test3(){
    for(i = 0; i < STRESS_TEST_ITERATIONS; i++) {
        v->push_back(v, &i);
    }
    for(i = 0; i < STRESS_TEST_ITERATIONS; i++) {
        w->push_back(w, &i);
    }
    assert_true(v->equal(v, w));
    assert_true(w->equal(w, v));
    int temp = *(int *)w->back(w);
    w->pop_back(w);
    assert_false(w->equal(w, v));
    w->push_back(w, &temp);
    assert_true(w->equal(w, v));
    w->erase(w, w->size(w)-1, 1);
    w->push_back(w, &temp);
    assert_true(w->equal(w, v));
}

void test4(){
    int a = 5;
    int b = 843;
    int c = 12;

    v->push_back(v, &a);
    v->push_back(v, &b);
    v->push_back(v, &c);

    int *temp = malloc(1);
    v->get(v, 0, (void *)&temp);
    assert_int_equal(a, *(int *)temp);
    v->get(v, 1, (void *)&temp);
    assert_int_equal(b, *(int *)temp);
    v->get(v, 2, (void *)&temp);
    assert_int_equal(c, *(int *)temp);

    // be sure that v is alive
    assert_int_equal(a, *(int *)v->at(v,0));
    assert_int_equal(b, *(int *)v->at(v,1));
    assert_int_equal(c, *(int *)v->at(v,2));

    v->sort(v);
    v->print(v);
    //assert_int_equal(*(int *)v->at(v,2), b);
    free(temp);
}

void test5(){
    assert_non_null(sv);
    char *sp;
    sp = "Keep";
    sv->push_back(sv, sp); // Keep
    sp = "on";
    sv->push_back(sv, sp); // Keep, on
    sp = "Rock!";
    sv->push_back(sv, sp); // Keep, on, Rock
    assert_false(sv->empty(sv));
    //sv->print(sv);
    assert_string_equal((char *)sv->back(sv), "Rock!");
    sv->pop_back(sv); // Keep, on
    //sv->print(sv);
    assert_false(sv->empty(sv));
    assert_string_equal((char *)sv->back(sv), "on");
    sp = "Hey kids!";
    sv->push_front(sv, sp); // Hey kids! , Keep, on
    //sv->print(sv);
    assert_string_equal((char *)sv->front(sv), "Hey kids!");
    sv->pop_back(sv); // Hey kids! , Keep
    //sv->print(sv);
    assert_false(sv->empty(sv));
    assert_string_equal((char *)sv->back(sv), "Keep");
    sv->pop_back(sv); // Hey kids!
    //sv->print(sv);
    assert_false(sv->empty(sv));    
    assert_string_equal((char *)sv->front(sv), "Hey kids!");
    assert_string_equal((char *)sv->back(sv), "Hey kids!");
    sv->pop_back(sv); // nothing
    //sv->print(sv);
    assert_true(sv->empty(sv));
}

void StressTestFor_PushBackPopBack(){
    // let's do some stress testing
    char *str[7] = {"Keep", "on", "Rock", "in", "the", "free", "world!"};
    for(i = 0; i < STRESS_TEST_ITERATIONS; i++) {
        assert_int_equal(sv->size(sv), i);
        sv->push_back(sv, str[i % 7]);
        assert_int_equal(sv->size(sv), i+1);
        assert_string_equal((char *)sv->at(sv, i), str[i % 7]);
    }
    int j;
    sv->fprint(sv, stdout);
    assert_int_equal(sv->size(sv), STRESS_TEST_ITERATIONS);
    printf("capacity is now: %zu\n", sv->capacity(sv));
    for(j = STRESS_TEST_ITERATIONS - 1; j >= 0; j--) {
        assert_false(sv->empty(sv));
        assert_int_equal(sv->size(sv), j+1);
        assert_string_equal((char *)sv->back(sv), str[j % 7]);
        assert_string_equal((char *)sv->at(sv, j), str[j % 7]);
        sv->pop_back(sv);
        assert_int_equal(sv->size(sv), j);
    }      
    assert_true(sv->empty(sv));
}

void StressTestFor_PushFrontPopFront(){
    // let's do some stress testing
    char *str[7] = {"Keep", "on", "Rock", "in", "the", "free", "world!"};
    for(i = 0; i < STRESS_TEST_ITERATIONS; i++) {
        assert_int_equal(sv->size(sv), i);
        sv->push_front(sv, str[i % 7]);
        assert_int_equal(sv->size(sv), i+1);
    }
    sv->print(sv);
    int j;
    for(j = STRESS_TEST_ITERATIONS - 1; j >= 0; j--) {
        assert_false(sv->empty(sv));
        assert_int_equal(sv->size(sv), j+1);
        assert_string_equal((char *)sv->front(sv), str[j % 7]);
        sv->pop_front(sv);
        assert_int_equal(sv->size(sv), j);
    }      
    assert_true(sv->empty(sv));
}

void TestFor_SwapEqual(){
    // let's do some stress testing
    char *str[7] = {"Keep", "on", "Rock", "in", "the", "free", "world!"};
    for(i = 0; i < 7; i++) {
        sv->push_back(sv, str[i % 7]);
        assert_int_equal(sv->size(sv), i+1);
    }
    printf("sv = ");sv->fprint(sv, stdout);
    //sv->assign(sv,sw);
    //sv->clear(sv);
    for(i = 0; i < 7; i++) {
        sw->push_front(sw, str[i % 7]);
        assert_int_equal(sw->size(sw), i+1);
    }
    printf("sw = "); sw->fprint(sw, stdout);
    assert_false(sv->equal(sv, sw));
    assert_false(sw->equal(sw, sv));
    size_t k = 0;
    size_t j = sw->size(sw) - 1;
    while (k < j){
        sw->swap(sw, k, j);
        k ++;
        j --;
    }
    printf("%s\n", "After Swap");
    printf("sv = ");sv->fprint(sv, stdout);
    printf("sw = "); sw->fprint(sw, stdout);
    assert_true(sv->equal(sv, sw));
    assert_true(sw->equal(sw, sv));    
}

void TestFor_AssignSetClear(){
    // let's do some stress testing
    char *str[7] = {"Keep", "on", "Rock", "in", "the", "free", "world!"};
    for(i = 0; i < 7; i++) {
        sv->push_back(sv, str[i % 7]);
        assert_int_equal(sv->size(sv), i+1);
    }
    printf("sw = "); sw->fprint(sw, stdout);
    sw->assign(sw,sv);
    printf("sw = "); sw->fprint(sw, stdout);
    assert_true(sw->equal(sw, sv));
    assert_true(sv->equal(sv, sw));
    sw->clear(sw);
    assert_false(sw->equal(sw, sv));
    assert_false(sv->equal(sv, sw));
    printf("after clear, sw = "); sw->fprint(sw, stdout);      
    for(i = 0; i < 7; i++) {
        sw->push_back(sw, str[i % 7]);
        assert_int_equal(sw->size(sw), i+1);
    }
    printf("after push back, sw = "); sw->fprint(sw, stdout);
    assert_true(sv->equal(sv, sw));
    for(i = 0; i < 7; i++) {
        sw->set(sw, i, str[(6-i) % 7]);
    }
    printf("after set, sw = "); sw->fprint(sw, stdout);
    assert_false(sw->equal(sw, sv));
    for(i = 0; i < 7; i++) {
        sw->set(sw, i, str[i % 7]);
    }
    printf("after set again, sw = "); sw->fprint(sw, stdout);
    assert_true(sw->equal(sw, sv));
    assert_true(sv->equal(sv, sw));
    sw->push_back(sw, str[0]);
    sw->push_back(sw, str[3]);

    printf("after push back, sw = "); sw->fprint(sw, stdout);
    sw->assign(sw, sv);
    printf("after assign, sw = "); sw->fprint(sw, stdout);
    assert_true(sw->equal(sw, sv));
    assert_true(sv->equal(sv, sw));   
}

void TestFor_FindAndSort(){
    // let's do some stress testing
    char *str[7] = {"keep", "on", "rock", "in", "the", "free", "world!"};
    for(i = 0; i < 7; i++) {
        sv->push_back(sv, str[i % 7]);
        assert_int_equal(sv->size(sv), i+1);
    }
    printf("sv = "); sv->print(sv);
    char *temp = "rock"; 
    int r = sv->find(sv, temp);
    assert_int_equal(r, 2);
    temp = "noentry";
    r = sv->find(sv, temp);
    assert_int_equal(r, -1);
    sv->sort(sv);
    printf("after sort, sv = "); sv->print(sv);
    assert_string_equal((char *)sv->at(sv, 0), "free");
}

void test7(){
    char *str[7] = {"Keep", "on", "Rock", "in", "the", "free", "world!"};
    for(i = 0; i < STRESS_TEST_ITERATIONS; i++) {
        sv->push_back(sv, str[i%7]);
    }
    int j;
    for(j = 0; j < STRESS_TEST_ITERATIONS; j++) {
        sw->push_back(sw, str[j%7]);
    }
    assert_true(sv->equal(sv, sw));
    assert_true(sw->equal(sw, sv));

    char *sp = malloc(10);
    sw->get(sw, sw->size(sw)-1, (void *)&sp);
    sw->pop_back(sw);
    assert_false(sw->equal(sw, sv));
    sw->push_back(sw, sp);
    assert_true(sw->equal(sw, sv));
    sw->erase(sw, sw->size(sw)-1, 1);
    sw->push_back(sw, sp);
    assert_true(sw->equal(sw, sv));
    free(sp);
}

void ExpectErrorTest(){
    for(i=0; i<5; i++) v->push_back(v, &i);
    assert_int_equal(v->size(v), 5);
    printf("********** Expecting Error logs ***********\n");
    int k = 99;
    int wstatus;
    pid_t pid = fork();
    if(!pid){
        v->set(v,6,&k); // should raise an error 
    }
    pid_t r = wait(&wstatus);
    assert_int_equal(r, pid);
    assert_true(WIFEXITED(wstatus));
    assert_int_equal(WEXITSTATUS(wstatus), 255);
    pid = fork();
    if(!pid){
        v->erase(v,2,5); // should raise an error 
    }
    r = wait(&wstatus);
    assert_true(WIFEXITED(wstatus));
    assert_int_equal(WEXITSTATUS(wstatus), 255);
    pid = fork();
    if(!pid){
        v->erase(v,5,1); // should raise an error 
    }
    r = wait(&wstatus);
    assert_true(WIFEXITED(wstatus));
    assert_int_equal(WEXITSTATUS(wstatus), 255);
    pid = fork();
    if(!pid){
        v->at(v,5); // should raise an error 
    }
    r = wait(&wstatus);
    assert_true(WIFEXITED(wstatus));
    assert_int_equal(WEXITSTATUS(wstatus), 255);
    pid = fork();
    if(!pid){
        v->get(v, 8, (void *)&k); // should raise an error 
    }
    r = wait(&wstatus);
    assert_true(WIFEXITED(wstatus));
    assert_int_equal(WEXITSTATUS(wstatus), 255);
    pid = fork();
    if(!pid){
        v->set(v, 5, &k); // should raise an error 
    }
    r = wait(&wstatus);
    assert_true(WIFEXITED(wstatus));
    assert_int_equal(WEXITSTATUS(wstatus), 255);
    pid = fork();
    if(!pid){
        v->swap(v, 1, 5); // should raise an error 
    }
    r = wait(&wstatus);
    assert_true(WIFEXITED(wstatus));
    assert_int_equal(WEXITSTATUS(wstatus), 255);
    pid = fork();
    if(!pid){
        v->insert(v, 5, &k); // should raise an error 
    }
    r = wait(&wstatus);
    assert_true(WIFEXITED(wstatus));
    assert_int_equal(WEXITSTATUS(wstatus), 255);
}


int main(void)
{
  const struct CMUnitTest tests[] = {
    cmocka_unit_test_setup_teardown(test1, setup, teardown),
    cmocka_unit_test_setup_teardown(test2, setup, teardown),
    cmocka_unit_test_setup_teardown(test3, setup, teardown),
    cmocka_unit_test_setup_teardown(test4, setup, teardown),
    cmocka_unit_test_setup_teardown(test5, setup, teardown),
    cmocka_unit_test_setup_teardown(StressTestFor_PushBackPopBack, setup, teardown),
    cmocka_unit_test_setup_teardown(test7, setup, teardown),
    cmocka_unit_test_setup_teardown(ExpectErrorTest, setup, teardown),
    cmocka_unit_test_setup_teardown(StressTestFor_PushFrontPopFront, setup, teardown),
    cmocka_unit_test_setup_teardown(TestFor_SwapEqual, setup, teardown),
    cmocka_unit_test_setup_teardown(TestFor_AssignSetClear, setup, teardown),
    cmocka_unit_test_setup_teardown(TestFor_FindAndSort, setup, teardown),
};

  return cmocka_run_group_tests(tests, NULL, NULL);
}
