#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <stdbool.h>

#include "gdsp_queue.h"

/* =========================
   Instanciations
   ========================= */

DECLARE_GDSP_QUEUE(int, int)
BUILD_GDSP_QUEUE(int, int)

typedef struct Person
{
    char *name;
    int age;
} Person;

DECLARE_GDSP_QUEUE(Person, person)
BUILD_GDSP_QUEUE(Person, person)

/* =========================
   Outils
   ========================= */

static int tests_run = 0;
static int tests_passed = 0;

#define RUN_TEST(fn)                          \
    do {                                      \
        printf("Running %-45s", #fn);         \
        fflush(stdout);                       \
        tests_run++;                          \
        fn();                                 \
        tests_passed++;                       \
        printf(" [OK]\n");                    \
    } while (0)

static char *my_strdup(const char *src)
{
    size_t len = strlen(src) + 1;
    char *copy = malloc(len);
    assert(copy != NULL);
    memcpy(copy, src, len);
    return copy;
}

/* =========================
   Destroy callbacks
   ========================= */

static int int_destroy_calls = 0;

static void destroy_int(int *x)
{
    (void)x;
    int_destroy_calls++;
}

static int person_destroy_calls = 0;

static void destroy_person(Person *p)
{
    person_destroy_calls++;
    if (p && p->name)
    {
        free(p->name);
        p->name = NULL;
    }
}

/* =========================
   Helpers
   ========================= */

static Person make_person(const char *name, int age)
{
    Person p;
    p.name = my_strdup(name);
    p.age = age;
    return p;
}

/* =========================
   Tests int queue
   ========================= */

static void test_create_empty(void)
{
    int_gdsp_queue *q = int_gdsp_queue_create(NULL);
    assert(q != NULL);

    assert(int_gdsp_queue_empty(q) == true);
    assert(int_gdsp_queue_size(q) == 0);
    assert(int_gdsp_queue_peek(q) == NULL);

    int out = 777;
    assert(int_gdsp_queue_pop(q, &out) == false);
    assert(out == 777);

    int_gdsp_queue_destroy(q);
}

static void test_push_one_peek(void)
{
    int_gdsp_queue *q = int_gdsp_queue_create(NULL);
    assert(q != NULL);

    assert(int_gdsp_queue_push(q, 42) == true);
    assert(int_gdsp_queue_empty(q) == false);
    assert(int_gdsp_queue_size(q) == 1);

    int *front = int_gdsp_queue_peek(q);
    assert(front != NULL);
    assert(*front == 42);

    int_gdsp_queue_destroy(q);
}

static void test_push_many_peek_oldest(void)
{
    int_gdsp_queue *q = int_gdsp_queue_create(NULL);
    assert(q != NULL);

    assert(int_gdsp_queue_push(q, 10) == true);
    assert(int_gdsp_queue_push(q, 20) == true);
    assert(int_gdsp_queue_push(q, 30) == true);
    assert(int_gdsp_queue_push(q, 40) == true);

    assert(int_gdsp_queue_size(q) == 4);

    int *front = int_gdsp_queue_peek(q);
    assert(front != NULL);
    assert(*front == 10);

    int_gdsp_queue_destroy(q);
}

static void test_pop_fifo(void)
{
    int_gdsp_queue *q = int_gdsp_queue_create(NULL);
    assert(q != NULL);

    assert(int_gdsp_queue_push(q, 10) == true);
    assert(int_gdsp_queue_push(q, 20) == true);
    assert(int_gdsp_queue_push(q, 30) == true);

    int out = 0;

    assert(int_gdsp_queue_pop(q, &out) == true);
    assert(out == 10);
    assert(int_gdsp_queue_size(q) == 2);

    assert(int_gdsp_queue_pop(q, &out) == true);
    assert(out == 20);
    assert(int_gdsp_queue_size(q) == 1);

    assert(int_gdsp_queue_pop(q, &out) == true);
    assert(out == 30);
    assert(int_gdsp_queue_size(q) == 0);

    assert(int_gdsp_queue_empty(q) == true);
    assert(int_gdsp_queue_peek(q) == NULL);

    assert(int_gdsp_queue_pop(q, &out) == false);

    int_gdsp_queue_destroy(q);
}

static void test_pop_without_output_buffer(void)
{
    int_gdsp_queue *q = int_gdsp_queue_create(NULL);
    assert(q != NULL);

    assert(int_gdsp_queue_push(q, 123) == true);
    assert(int_gdsp_queue_size(q) == 1);

    assert(int_gdsp_queue_pop(q, NULL) == true);
    assert(int_gdsp_queue_size(q) == 0);
    assert(int_gdsp_queue_empty(q) == true);

    int_gdsp_queue_destroy(q);
}

static void test_single_element_pop_updates_head_and_tail(void)
{
    int_gdsp_queue *q = int_gdsp_queue_create(NULL);
    assert(q != NULL);

    assert(int_gdsp_queue_push(q, 99) == true);
    assert(int_gdsp_queue_size(q) == 1);

    int out = 0;
    assert(int_gdsp_queue_pop(q, &out) == true);
    assert(out == 99);

    assert(int_gdsp_queue_size(q) == 0);
    assert(int_gdsp_queue_empty(q) == true);
    assert(int_gdsp_queue_peek(q) == NULL);

    assert(q->head == NULL);
    assert(q->tail == NULL);

    int_gdsp_queue_destroy(q);
}

static void test_clear(void)
{
    int_destroy_calls = 0;

    int_gdsp_queue *q = int_gdsp_queue_create(destroy_int);
    assert(q != NULL);

    for (int i = 0; i < 10; i++)
        assert(int_gdsp_queue_push(q, i) == true);

    assert(int_gdsp_queue_size(q) == 10);

    int_gdsp_queue_clear(q);

    assert(int_gdsp_queue_size(q) == 0);
    assert(int_gdsp_queue_empty(q) == true);
    assert(int_gdsp_queue_peek(q) == NULL);
    assert(q->head == NULL);
    assert(q->tail == NULL);
    assert(int_destroy_calls == 10);

    int_gdsp_queue_destroy(q);
}

static void test_reuse_after_clear(void)
{
    int_gdsp_queue *q = int_gdsp_queue_create(NULL);
    assert(q != NULL);

    assert(int_gdsp_queue_push(q, 1) == true);
    assert(int_gdsp_queue_push(q, 2) == true);
    assert(int_gdsp_queue_push(q, 3) == true);

    int_gdsp_queue_clear(q);

    assert(int_gdsp_queue_size(q) == 0);
    assert(int_gdsp_queue_push(q, 99) == true);

    int *front = int_gdsp_queue_peek(q);
    assert(front != NULL);
    assert(*front == 99);

    int out;
    assert(int_gdsp_queue_pop(q, &out) == true);
    assert(out == 99);

    int_gdsp_queue_destroy(q);
}

static void test_large_volume(void)
{
    int_gdsp_queue *q = int_gdsp_queue_create(NULL);
    assert(q != NULL);

    const int n = 5000;

    for (int i = 0; i < n; i++)
        assert(int_gdsp_queue_push(q, i) == true);

    assert(int_gdsp_queue_size(q) == (size_t)n);

    for (int i = 0; i < n; i++)
    {
        int out = -1;
        assert(int_gdsp_queue_pop(q, &out) == true);
        assert(out == i);
    }

    assert(int_gdsp_queue_empty(q) == true);
    assert(int_gdsp_queue_size(q) == 0);

    int_gdsp_queue_destroy(q);
}

static void test_null_queue_behavior(void)
{
    assert(int_gdsp_queue_empty(NULL) == true);
    assert(int_gdsp_queue_size(NULL) == 0);
    assert(int_gdsp_queue_peek(NULL) == NULL);
    assert(int_gdsp_queue_pop(NULL, NULL) == false);
    assert(int_gdsp_queue_push(NULL, 5) == false);

    int_gdsp_queue_clear(NULL);
    int_gdsp_queue_destroy(NULL);
}

/* =========================
   Tests Person queue
   ========================= */

static void test_person_peek(void)
{
    person_gdsp_queue *q = person_gdsp_queue_create(destroy_person);
    assert(q != NULL);

    assert(person_gdsp_queue_push(q, make_person("Alice", 21)) == true);
    assert(person_gdsp_queue_push(q, make_person("Bob", 30)) == true);

    Person *front = person_gdsp_queue_peek(q);
    assert(front != NULL);
    assert(strcmp(front->name, "Alice") == 0);
    assert(front->age == 21);

    person_gdsp_queue_destroy(q);
}

static void test_person_pop_fifo(void)
{
    person_destroy_calls = 0;

    person_gdsp_queue *q = person_gdsp_queue_create(destroy_person);
    assert(q != NULL);

    assert(person_gdsp_queue_push(q, make_person("Alice", 21)) == true);
    assert(person_gdsp_queue_push(q, make_person("Bob", 30)) == true);
    assert(person_gdsp_queue_push(q, make_person("Charlie", 40)) == true);

    Person out;

    assert(person_gdsp_queue_pop(q, &out) == true);
    assert(strcmp(out.name, "Alice") == 0);
    assert(out.age == 21);
    free(out.name);

    assert(person_gdsp_queue_pop(q, &out) == true);
    assert(strcmp(out.name, "Bob") == 0);
    assert(out.age == 30);
    free(out.name);

    assert(person_gdsp_queue_pop(q, &out) == true);
    assert(strcmp(out.name, "Charlie") == 0);
    assert(out.age == 40);
    free(out.name);

    assert(person_gdsp_queue_empty(q) == true);

    /* pop ne doit pas appeler destroy_data */
    assert(person_destroy_calls == 0);

    person_gdsp_queue_destroy(q);
}

static void test_person_clear_calls_destroy(void)
{
    person_destroy_calls = 0;

    person_gdsp_queue *q = person_gdsp_queue_create(destroy_person);
    assert(q != NULL);

    assert(person_gdsp_queue_push(q, make_person("A", 10)) == true);
    assert(person_gdsp_queue_push(q, make_person("B", 20)) == true);
    assert(person_gdsp_queue_push(q, make_person("C", 30)) == true);

    person_gdsp_queue_clear(q);

    assert(person_gdsp_queue_empty(q) == true);
    assert(person_gdsp_queue_size(q) == 0);
    assert(person_destroy_calls == 3);

    person_gdsp_queue_destroy(q);
}

static void test_person_destroy_remaining_only(void)
{
    person_destroy_calls = 0;

    person_gdsp_queue *q = person_gdsp_queue_create(destroy_person);
    assert(q != NULL);

    assert(person_gdsp_queue_push(q, make_person("A", 1)) == true);
    assert(person_gdsp_queue_push(q, make_person("B", 2)) == true);
    assert(person_gdsp_queue_push(q, make_person("C", 3)) == true);

    Person out;
    assert(person_gdsp_queue_pop(q, &out) == true);
    assert(strcmp(out.name, "A") == 0);
    free(out.name);

    person_gdsp_queue_destroy(q);

    /* Il restait B et C */
    assert(person_destroy_calls == 2);
}

/* =========================
   Main
   ========================= */

int main(void)
{
    RUN_TEST(test_create_empty);
    RUN_TEST(test_push_one_peek);
    RUN_TEST(test_push_many_peek_oldest);
    RUN_TEST(test_pop_fifo);
    RUN_TEST(test_pop_without_output_buffer);
    RUN_TEST(test_single_element_pop_updates_head_and_tail);
    RUN_TEST(test_clear);
    RUN_TEST(test_reuse_after_clear);
    RUN_TEST(test_large_volume);
    RUN_TEST(test_null_queue_behavior);

    RUN_TEST(test_person_peek);
    RUN_TEST(test_person_pop_fifo);
    RUN_TEST(test_person_clear_calls_destroy);
    RUN_TEST(test_person_destroy_remaining_only);

    printf("\n%d/%d tests passed successfully.\n", tests_passed, tests_run);
    return 0;
}