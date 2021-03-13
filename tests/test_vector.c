#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <stdlib.h>
#include <stdint.h>
#include <cmocka.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <errno.h>
#include <sys/wait.h>
#include <unistd.h>
#include <math.h>
#include "log.h"
#include "vector.h"

#define static
#define UNUSED(x) (void)(x)
#define STRESS_TEST_ITERATIONS (100000)

static void set_header(FILE *fp, const char *testname){
    fprintf(fp, "\n%s\n", "#######################################");
    fprintf(fp, "\t%s\n", testname);
    fprintf(fp, "%s\n\n", "#######################################");    
}

/* Declare the variables your tests want to use. */

static vector v;
static vector w;
static vector sv;
static vector sw;
static int i;
static FILE *fp;
static char logbuf[BUFSIZ];
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
    assert_non_null(sv);
    assert_non_null(sw);
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

void BasicTestFor_PusbackAndPopBack(){
    set_header(fp, __func__);
    assert_non_null(v);
    i = 1;
    log_info("pushing i = %d", i);
    v->push_back(v, &i);
    i = 2;
    log_info("pushing i = %d", i);
    v->push_back(v, &i);
    i = 3;
    log_info("pushing i = %d", i);
    v->push_back(v, &i);
    v->fprint(v, fp);

    assert_false(v->empty(v));
    assert_int_equal(*(int *)v->back(v), 3);
    log_info("pop back");
    v->pop_back(v);
    v->fprint(v, fp);
    assert_false(v->empty(v));
    assert_int_equal(*(int *)v->back(v), 2);
    log_info("pop back");
    v->pop_back(v);
    v->fprint(v, fp);
    assert_false(v->empty(v));    
    assert_int_equal(*(int *)v->back(v), 1);
    log_info("pop back");
    v->pop_back(v);
    v->fprint(v, fp);
    assert_true(v->empty(v));
}

void StressTestFor_IntPushBackAndPopBack(){
    // stress testing
    set_header(fp , __func__);
    log_info("starting push backs");
    for(i = 0; i < STRESS_TEST_ITERATIONS; i++) {
        v->push_back(v, &i);
    }
    log_info("push back ends");
    v->fprint(v, fp);
    int j;
    log_info("starting pop backs");
    for(j = STRESS_TEST_ITERATIONS - 1; j >= 0; j--) {
        assert_false(v->empty(v));
        assert_int_equal(*(int *)v->back(v), j);
        v->pop_back(v);
    }
    log_info("pop back ends");
    v->fprint(v, fp);
    assert_true(v->empty(v));
}

void BasicTestFor_Erase(){
    set_header(fp , __func__);
    log_info("starting push backs for v");
    for(i = 0; i < 10; i++) {
        v->push_back(v, &i);
    }
    log_info("starting push backs for w");
    for(i = 0; i < 10; i++) {
        w->push_back(w, &i);
    }
    log_info("push backs ends");
    v->fprint(v, fp);
    w->fprint(w, fp);
    assert_true(v->equal(v, w));
    assert_true(w->equal(w, v));
    int temp = *(int *)w->back(w);
    log_info("Pop back from w");
    w->pop_back(w);
    w->fprint(w, fp);
    assert_false(w->equal(w, v));
    log_info("Push back w");
    w->push_back(w, &temp);
    w->fprint(w, fp);
    assert_true(w->equal(w, v));
    log_info("erasing last element from w");
    w->erase(w, w->size(w)-1, 1);
    w->fprint(w, fp);
    log_info("Push back w");
    w->push_back(w, &temp);
    v->fprint(v, fp);
    w->fprint(w, fp);
    assert_true(w->equal(w, v));
}

void BasicTestFor_IntSort(){
    set_header(fp , __func__);
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
    log_info("before sorting");
    v->fprint(v, fp);
    v->sort(v);
    log_info("after sorting");
    v->fprint(v, fp);
    assert_int_equal(*(int *)v->at(v,2), b);
    free(temp);
}

void BasicTestFor_BackFront(){
    set_header(fp , __func__);
    assert_non_null(sv);
    char *sp;
    log_info("Starting push backs");
    sp = "Keep";
    sv->push_back(sv, sp); // Keep
    sp = "on";
    sv->push_back(sv, sp); // Keep, on
    sp = "Rock!";
    sv->push_back(sv, sp); // Keep, on, Rock
    assert_false(sv->empty(sv));
    sv->fprint(sv, fp);
    assert_string_equal((char *)sv->back(sv), "Rock!");
    log_info("pop back");
    sv->pop_back(sv); // Keep, on
    sv->fprint(sv, fp);
    assert_false(sv->empty(sv));
    assert_string_equal((char *)sv->back(sv), "on");
    sp = "Hey kids!";
    log_info("push front");
    sv->push_front(sv, sp); // Hey kids! , Keep, on
    sv->fprint(sv, fp);
    assert_string_equal((char *)sv->front(sv), "Hey kids!");
    log_info("pop back");
    sv->pop_back(sv); // Hey kids! , Keep
    sv->fprint(sv, fp);
    assert_false(sv->empty(sv));
    assert_string_equal((char *)sv->back(sv), "Keep");
    log_info("pop back");   
    sv->pop_back(sv); // Hey kids!
    sv->fprint(sv, fp);
    assert_false(sv->empty(sv));    
    assert_string_equal((char *)sv->front(sv), "Hey kids!");
    assert_string_equal((char *)sv->back(sv), "Hey kids!");
    log_info("pop back");       
    sv->pop_back(sv); // nothing
    sv->fprint(sv, fp);
    assert_true(sv->empty(sv));
}

void StressTestFor_PushBackPopBackCapacity(){
    // let's do some stress testing
    set_header(fp , __func__);
    char *str[7] = {"Keep", "on", "Rock", "in", "the", "free", "world!"};    
    log_info("Stress test push_backs starts");
    for(i = 0; i < STRESS_TEST_ITERATIONS; i++) {
        assert_int_equal(sv->size(sv), i);
        sv->push_back(sv, str[i % 7]);
        assert_int_equal(sv->size(sv), i+1);
        assert_string_equal((char *)sv->at(sv, i), str[i % 7]);
    }
    log_info("Stress test push_backs ends");
    sv->fprint(sv, fp);
    int j;
    assert_int_equal(sv->size(sv), STRESS_TEST_ITERATIONS);
    double val = log((double)sv->size(sv))/(double)log(2);
    val = ceil(val);
    val = pow(2, val);
    log_info("Checking capacity");
    assert_int_equal(sv->capacity(sv), (size_t)val);
    log_info("Stress test pop_backs starts");
    for(j = STRESS_TEST_ITERATIONS - 1; j >= 0; j--) {
        assert_false(sv->empty(sv));
        assert_int_equal(sv->size(sv), j+1);
        assert_string_equal((char *)sv->back(sv), str[j % 7]);
        assert_string_equal((char *)sv->at(sv, j), str[j % 7]);
        sv->pop_back(sv);
        assert_int_equal(sv->size(sv), j);
    }
    log_info("Stress test pop_backs ends");
    sv->fprint(sv, fp);
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
    char *str[7] = {"Keep", "on", "Rock", "in", "the", "free", "world!"};
    for(i = 0; i < 7; i++) {
        sv->push_back(sv, str[i % 7]);
        assert_int_equal(sv->size(sv), i+1);
    }
    printf("sv = ");sv->fprint(sv, stdout);
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

void BasicTestFor_Get(){
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
    set_header(fp, __func__);
    for(i=0; i<5; i++) v->push_back(v, &i);
    assert_int_equal(v->size(v), 5);
    log_info("This tests checks whether we get exectuion stops in case of an index out of bound");
    log_warn("######### Expecting Error logs in stdout ###########");
    int k = 99;
    int wstatus;
    log_info("Testing set error");
    pid_t pid = fork();
    if(!pid){
        v->set(v,6,&k); // should raise an error 
    }
    pid_t r = wait(&wstatus);
    assert_int_equal(r, pid);
    assert_true(WIFEXITED(wstatus));
    assert_int_equal(WEXITSTATUS(wstatus), 255);
    log_info("Testing erase error");
    pid = fork();
    if(!pid){
        v->erase(v,2,5); // should raise an error 
    }
    r = wait(&wstatus);
    assert_true(WIFEXITED(wstatus));
    assert_int_equal(WEXITSTATUS(wstatus), 255);
    log_info("Testing erase error");
    pid = fork();
    if(!pid){
        v->erase(v,5,1); // should raise an error 
    }
    r = wait(&wstatus);
    assert_true(WIFEXITED(wstatus));
    assert_int_equal(WEXITSTATUS(wstatus), 255);
    log_info("Testing at error");
    pid = fork();
    if(!pid){
        v->at(v,5); // should raise an error 
    }
    r = wait(&wstatus);
    assert_true(WIFEXITED(wstatus));
    assert_int_equal(WEXITSTATUS(wstatus), 255);
    log_info("Testing get error");
    pid = fork();
    if(!pid){
        v->get(v, 8, (void *)&k); // should raise an error 
    }
    r = wait(&wstatus);
    assert_true(WIFEXITED(wstatus));
    assert_int_equal(WEXITSTATUS(wstatus), 255);
    log_info("Testing set error");
    pid = fork();
    if(!pid){
        v->set(v, 5, &k); // should raise an error 
    }
    r = wait(&wstatus);
    assert_true(WIFEXITED(wstatus));
    assert_int_equal(WEXITSTATUS(wstatus), 255);
    log_info("Testing swap error");
    pid = fork();
    if(!pid){
        v->swap(v, 1, 5); // should raise an error 
    }
    r = wait(&wstatus);
    assert_true(WIFEXITED(wstatus));
    assert_int_equal(WEXITSTATUS(wstatus), 255);
    log_info("Testing insert error");
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
    char filename[200];
    sprintf(filename, "%s/", TEST_LOGS_DIR);
    sprintf(filename+strlen(filename), "%s", "test_vector_logs.txt");
    printf("filename = %s\n", filename);
    fp = fopen(filename, "wb");
    if(!fp) fprintf(stderr, "%s", "failed to create a log file\n");
    int level = 0;
    log_add_fp(fp, level);
    const struct CMUnitTest tests[] = {
    cmocka_unit_test_setup_teardown(BasicTestFor_PusbackAndPopBack, setup, teardown),
    cmocka_unit_test_setup_teardown(StressTestFor_IntPushBackAndPopBack, setup, teardown),
    cmocka_unit_test_setup_teardown(BasicTestFor_Erase, setup, teardown),
    cmocka_unit_test_setup_teardown(BasicTestFor_IntSort, setup, teardown),
    cmocka_unit_test_setup_teardown(BasicTestFor_BackFront, setup, teardown),
    cmocka_unit_test_setup_teardown(StressTestFor_PushBackPopBackCapacity, setup, teardown),
    //cmocka_unit_test_setup_teardown(BasicTestFor_Get, setup, teardown),
    cmocka_unit_test_setup_teardown(ExpectErrorTest, setup, teardown),
    //cmocka_unit_test_setup_teardown(StressTestFor_PushFrontPopFront, setup, teardown),
    //cmocka_unit_test_setup_teardown(TestFor_SwapEqual, setup, teardown),
    //cmocka_unit_test_setup_teardown(TestFor_AssignSetClear, setup, teardown),
    //cmocka_unit_test_setup_teardown(TestFor_FindAndSort, setup, teardown),
};

  return cmocka_run_group_tests(tests, NULL, NULL);
}
