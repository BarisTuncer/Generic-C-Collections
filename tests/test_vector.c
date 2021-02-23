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
    int b = 12;
    int c = 848;

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

void test6(){
    // let's do some stress testing
    char *str[7] = {"Keep", "on", "Rock", "in", "the", "free", "world!"};
    for(i = 0; i < STRESS_TEST_ITERATIONS; i++) {
        sv->push_back(sv, str[i % 7]);
    }
    int j;
    // sv->print(sv);
    for(j = STRESS_TEST_ITERATIONS - 1; j >= 0; j--) {
        assert_false(sv->empty(sv));
        assert_string_equal((char *)sv->back(sv), str[j % 7]);
        sv->pop_back(sv);
    }      
    assert_true(sv->empty(sv));
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

void __wrap_exit(int status);
void __wrap_exit(int status){
    UNUSED(status);
    printf("Raising SIGUSR1. ");
    raise(SIGUSR1);
}
static jmp_buf env;
static void sig_handler(int signo){
if (signo == SIGUSR1){
    printf("Received SIGUSR1\n");
    assert_true(true);
    longjmp(env, 1);  // longjmp() ``returns'' to the state of the program when setjmp() was called.
    }
}
void test8(){
    signal(SIGUSR1, sig_handler);
    i = setjmp(env);      // This says to save the current state of the registers into env.
    if (i != 0) return;    
    v->set(v,1,&i);  // should raise an error
}

int main(void)
{
  const struct CMUnitTest tests[] = {
    cmocka_unit_test_setup_teardown(test1, setup, teardown),
    cmocka_unit_test_setup_teardown(test2, setup, teardown),
    cmocka_unit_test_setup_teardown(test3, setup, teardown),
    cmocka_unit_test_setup_teardown(test4, setup, teardown),
    cmocka_unit_test_setup_teardown(test5, setup, teardown),
    cmocka_unit_test_setup_teardown(test6, setup, teardown),
    cmocka_unit_test_setup_teardown(test7, setup, teardown),
    cmocka_unit_test_setup_teardown(test8, setup, teardown),
};

  return cmocka_run_group_tests(tests, NULL, NULL);
}
