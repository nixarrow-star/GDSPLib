#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <assert.h>
#include <time.h>

#include "gdsp_vector.h"
#include "gdsp_heap.h"

/* =========================================================
   DECLARATION / BUILD DES STRUCTURES GENERIQUES
   ========================================================= */

/*
    Adapte ces lignes si tes macros attendent d'autres noms.

    Ici :
    - TYPE     = int
    - HEAP     = int
    - VECTOR   = int

    Cela devrait générer :
    - int_gdsp_vector
    - int_gdsp_heap
    - int_gdsp_vector_create(...)
    - int_gdsp_heap_create(...)
    etc.
*/

DECLARE_GDSP_HEAP(int, int, int)

BUILD_GDSP_HEAP(int, int, int)

/* =========================================================
   OUTILS DE TEST
   ========================================================= */

static int g_destroy_count = 0;

static void int_destroy(int *x)
{
    /*
        Pour un int, il n'y a rien à libérer.
        On compte juste les appels pour tester destroy_data.
    */
    if (x)
        g_destroy_count++;
}

static int int_cmp_max(int a, int b)
{
    /*
        Max-heap :
        cmp(a, b) > 0 signifie que a est prioritaire sur b.
    */
    return (a > b) - (a < b);
}

static int int_cmp_min(int a, int b)
{
    /*
        Min-heap :
        on inverse la priorité.
    */
    return (b > a) - (b < a);
}

static void swap_int(int *a, int *b)
{
    int tmp = *a;
    *a = *b;
    *b = tmp;
}

static void sort_int_array_asc(int *arr, size_t n)
{
    /*
        Tri simple pour vérifier les résultats.
        O(n^2), mais suffisant pour les tests.
    */
    for (size_t i = 0; i < n; i++) {
        for (size_t j = i + 1; j < n; j++) {
            if (arr[j] < arr[i])
                swap_int(&arr[i], &arr[j]);
        }
    }
}

static void sort_int_array_desc(int *arr, size_t n)
{
    /*
        Tri décroissant pour comparer avec les pop d'une max-heap.
    */
    for (size_t i = 0; i < n; i++) {
        for (size_t j = i + 1; j < n; j++) {
            if (arr[j] > arr[i])
                swap_int(&arr[i], &arr[j]);
        }
    }
}

static bool vector_is_sorted_asc(int_gdsp_vector *vec)
{
    if (!vec)
        return false;

    size_t n = int_gdsp_vector_size(vec);

    if (n < 2)
        return true;

    for (size_t i = 1; i < n; i++) {
        int *prev = int_gdsp_vector_get_at(vec, i - 1);
        int *cur = int_gdsp_vector_get_at(vec, i);

        if (!prev || !cur)
            return false;

        if (*prev > *cur)
            return false;
    }

    return true;
}

static bool check_heap_invariant_vector(
    int_gdsp_vector *vec,
    int (*cmp)(int, int))
{
    /*
        Vérifie que pour chaque noeud i :
        vec[i] >= vec[left]
        vec[i] >= vec[right]
        selon le comparator cmp.
    */

    if (!vec || !cmp)
        return false;

    size_t n = int_gdsp_vector_size(vec);

    for (size_t i = 0; i < n; i++) {
        size_t left = 2 * i + 1;
        size_t right = 2 * i + 2;

        int *parent = int_gdsp_vector_get_at(vec, i);

        if (!parent)
            return false;

        if (left < n) {
            int *child = int_gdsp_vector_get_at(vec, left);

            if (!child)
                return false;

            if (cmp(*child, *parent) > 0) {
                fprintf(stderr,
                        "Heap invariant failed at index %zu -> left child %zu\n",
                        i,
                        left);
                fprintf(stderr,
                        "parent = %d, child = %d\n",
                        *parent,
                        *child);
                return false;
            }
        }

        if (right < n) {
            int *child = int_gdsp_vector_get_at(vec, right);

            if (!child)
                return false;

            if (cmp(*child, *parent) > 0) {
                fprintf(stderr,
                        "Heap invariant failed at index %zu -> right child %zu\n",
                        i,
                        right);
                fprintf(stderr,
                        "parent = %d, child = %d\n",
                        *parent,
                        *child);
                return false;
            }
        }
    }

    return true;
}

static bool check_heap_invariant_heap(
    int_gdsp_heap *heap,
    int (*cmp)(int, int))
{
    if (!heap)
        return false;

    /*
        Ce test suppose que ta structure heap contient un champ :
            heap->vec

        Si tu as rendu la structure opaque, il faudra exposer une fonction
        de debug ou ignorer ce test.
    */
    return check_heap_invariant_vector(heap->vec, cmp);
}

static void print_test_start(const char *name)
{
    printf("[TEST] %s...\n", name);
}

static void print_test_ok(const char *name)
{
    printf("[OK]   %s\n\n", name);
}

/* =========================================================
   TEST 1 : CREATION / DESTRUCTION
   ========================================================= */

static void test_create_destroy(void)
{
    const char *name = "create / destroy";
    print_test_start(name);

    int_gdsp_heap *heap = int_gdsp_heap_create(int_cmp_max, NULL);
    assert(heap != NULL);
    assert(int_gdsp_heap_size(heap) == 0);
    assert(int_gdsp_heap_peek(heap) == NULL);

    int_gdsp_heap_destroy(heap);

    print_test_ok(name);
}

/* =========================================================
   TEST 2 : POP SUR HEAP VIDE
   ========================================================= */

static void test_empty_heap(void)
{
    const char *name = "empty heap behavior";
    print_test_start(name);

    int_gdsp_heap *heap = int_gdsp_heap_create(int_cmp_max, NULL);
    assert(heap != NULL);

    int out = 0;

    assert(int_gdsp_heap_size(heap) == 0);
    assert(int_gdsp_heap_peek(heap) == NULL);
    assert(int_gdsp_heap_pop(heap, &out) == false);

    int_gdsp_heap_destroy(heap);

    print_test_ok(name);
}

/* =========================================================
   TEST 3 : PUSH SIMPLE + PEEK
   ========================================================= */

static void test_push_peek_simple(void)
{
    const char *name = "push + peek simple";
    print_test_start(name);

    int_gdsp_heap *heap = int_gdsp_heap_create(int_cmp_max, NULL);
    assert(heap != NULL);

    assert(int_gdsp_heap_push(heap, 10));
    assert(int_gdsp_heap_size(heap) == 1);
    assert(*int_gdsp_heap_peek(heap) == 10);
    assert(check_heap_invariant_heap(heap, int_cmp_max));

    assert(int_gdsp_heap_push(heap, 5));
    assert(int_gdsp_heap_size(heap) == 2);
    assert(*int_gdsp_heap_peek(heap) == 10);
    assert(check_heap_invariant_heap(heap, int_cmp_max));

    assert(int_gdsp_heap_push(heap, 42));
    assert(int_gdsp_heap_size(heap) == 3);
    assert(*int_gdsp_heap_peek(heap) == 42);
    assert(check_heap_invariant_heap(heap, int_cmp_max));

    assert(int_gdsp_heap_push(heap, 1));
    assert(int_gdsp_heap_size(heap) == 4);
    assert(*int_gdsp_heap_peek(heap) == 42);
    assert(check_heap_invariant_heap(heap, int_cmp_max));

    int_gdsp_heap_destroy(heap);

    print_test_ok(name);
}

/* =========================================================
   TEST 4 : POP ORDRE MAX-HEAP
   ========================================================= */

static void test_pop_order_max_heap(void)
{
    const char *name = "pop order max-heap";
    print_test_start(name);

    int values[] = {
        10, 4, 7, 30, 2, 100, 3, 3, 99, -5, 0, 42
    };

    size_t n = sizeof(values) / sizeof(values[0]);

    int expected[sizeof(values) / sizeof(values[0])];

    for (size_t i = 0; i < n; i++)
        expected[i] = values[i];

    sort_int_array_desc(expected, n);

    int_gdsp_heap *heap = int_gdsp_heap_create(int_cmp_max, NULL);
    assert(heap != NULL);

    for (size_t i = 0; i < n; i++) {
        assert(int_gdsp_heap_push(heap, values[i]));
        assert(check_heap_invariant_heap(heap, int_cmp_max));
    }

    assert(int_gdsp_heap_size(heap) == n);

    for (size_t i = 0; i < n; i++) {
        int out = 0;

        assert(int_gdsp_heap_pop(heap, &out));
        assert(out == expected[i]);
        assert(check_heap_invariant_heap(heap, int_cmp_max));
    }

    assert(int_gdsp_heap_size(heap) == 0);
    assert(int_gdsp_heap_peek(heap) == NULL);

    int out = 0;
    assert(int_gdsp_heap_pop(heap, &out) == false);

    int_gdsp_heap_destroy(heap);

    print_test_ok(name);
}

/* =========================================================
   TEST 5 : MIN-HEAP AVEC COMPARATOR INVERSE
   ========================================================= */

static void test_min_heap(void)
{
    const char *name = "min-heap via reversed comparator";
    print_test_start(name);

    int values[] = {
        50, 20, -10, 7, 7, 100, 0, -99, 3
    };

    size_t n = sizeof(values) / sizeof(values[0]);

    int expected[sizeof(values) / sizeof(values[0])];

    for (size_t i = 0; i < n; i++)
        expected[i] = values[i];

    sort_int_array_asc(expected, n);

    int_gdsp_heap *heap = int_gdsp_heap_create(int_cmp_min, NULL);
    assert(heap != NULL);

    for (size_t i = 0; i < n; i++) {
        assert(int_gdsp_heap_push(heap, values[i]));
        assert(check_heap_invariant_heap(heap, int_cmp_min));
    }

    for (size_t i = 0; i < n; i++) {
        int out = 0;

        assert(int_gdsp_heap_pop(heap, &out));
        assert(out == expected[i]);
        assert(check_heap_invariant_heap(heap, int_cmp_min));
    }

    assert(int_gdsp_heap_size(heap) == 0);

    int_gdsp_heap_destroy(heap);

    print_test_ok(name);
}

/* =========================================================
   TEST 6 : DOUBLONS
   ========================================================= */

static void test_duplicates(void)
{
    const char *name = "duplicates";
    print_test_start(name);

    int_gdsp_heap *heap = int_gdsp_heap_create(int_cmp_max, NULL);
    assert(heap != NULL);

    for (int i = 0; i < 100; i++) {
        assert(int_gdsp_heap_push(heap, 42));
        assert(*int_gdsp_heap_peek(heap) == 42);
        assert(check_heap_invariant_heap(heap, int_cmp_max));
    }

    assert(int_gdsp_heap_size(heap) == 100);

    for (int i = 0; i < 100; i++) {
        int out = 0;

        assert(int_gdsp_heap_pop(heap, &out));
        assert(out == 42);
        assert(check_heap_invariant_heap(heap, int_cmp_max));
    }

    assert(int_gdsp_heap_size(heap) == 0);

    int_gdsp_heap_destroy(heap);

    print_test_ok(name);
}

/* =========================================================
   TEST 7 : VALEURS NEGATIVES
   ========================================================= */

static void test_negative_values(void)
{
    const char *name = "negative values";
    print_test_start(name);

    int values[] = {
        -1, -50, -3, -999, -4, -2, -100
    };

    size_t n = sizeof(values) / sizeof(values[0]);

    int expected[] = {
        -1, -2, -3, -4, -50, -100, -999
    };

    int_gdsp_heap *heap = int_gdsp_heap_create(int_cmp_max, NULL);
    assert(heap != NULL);

    for (size_t i = 0; i < n; i++) {
        assert(int_gdsp_heap_push(heap, values[i]));
        assert(check_heap_invariant_heap(heap, int_cmp_max));
    }

    for (size_t i = 0; i < n; i++) {
        int out = 0;

        assert(int_gdsp_heap_pop(heap, &out));
        assert(out == expected[i]);
        assert(check_heap_invariant_heap(heap, int_cmp_max));
    }

    int_gdsp_heap_destroy(heap);

    print_test_ok(name);
}

/* =========================================================
   TEST 8 : GROS TEST PSEUDO-ALEATOIRE
   ========================================================= */

static void test_random_stress(void)
{
    const char *name = "random stress test";
    print_test_start(name);

    enum { N = 5000 };

    int *values = malloc(sizeof(int) * N);
    int *expected = malloc(sizeof(int) * N);

    assert(values != NULL);
    assert(expected != NULL);

    srand(12345);

    int_gdsp_heap *heap = int_gdsp_heap_create(int_cmp_max, NULL);
    assert(heap != NULL);

    for (int i = 0; i < N; i++) {
        int v = (rand() % 20001) - 10000;

        values[i] = v;
        expected[i] = v;

        assert(int_gdsp_heap_push(heap, v));
        assert(check_heap_invariant_heap(heap, int_cmp_max));
    }

    sort_int_array_desc(expected, N);

    assert(int_gdsp_heap_size(heap) == N);

    for (int i = 0; i < N; i++) {
        int out = 0;

        assert(int_gdsp_heap_pop(heap, &out));

        if (out != expected[i]) {
            fprintf(stderr,
                    "Random stress mismatch at index %d: got %d, expected %d\n",
                    i,
                    out,
                    expected[i]);
            assert(false);
        }

        assert(check_heap_invariant_heap(heap, int_cmp_max));
    }

    assert(int_gdsp_heap_size(heap) == 0);

    int_gdsp_heap_destroy(heap);

    free(values);
    free(expected);

    print_test_ok(name);
}

/* =========================================================
   TEST 9 : HEAPIFY DEPUIS UN VECTOR EXISTANT
   ========================================================= */

static void test_heapify(void)
{
    const char *name = "heapify existing vector";
    print_test_start(name);

    int values[] = {
        9, 1, 8, 2, 7, 3, 6, 4, 5, 0, -1, 100
    };

    size_t n = sizeof(values) / sizeof(values[0]);

    int expected[sizeof(values) / sizeof(values[0])];

    for (size_t i = 0; i < n; i++)
        expected[i] = values[i];

    sort_int_array_desc(expected, n);

    int_gdsp_vector *vec = int_gdsp_vector_create(NULL);
    assert(vec != NULL);

    for (size_t i = 0; i < n; i++)
        assert(int_gdsp_vector_push_back(vec, values[i]));

    int_gdsp_heap *heap = int_gdsp_heap_heapify(vec, int_cmp_max);
    assert(heap != NULL);

    /*
        Après heapify, on suppose que la heap est propriétaire de vec.
        Donc on ne fera pas int_gdsp_vector_destroy(vec) directement.
    */

    assert(int_gdsp_heap_size(heap) == n);
    assert(check_heap_invariant_heap(heap, int_cmp_max));
    assert(*int_gdsp_heap_peek(heap) == expected[0]);

    for (size_t i = 0; i < n; i++) {
        int out = 0;

        assert(int_gdsp_heap_pop(heap, &out));
        assert(out == expected[i]);
        assert(check_heap_invariant_heap(heap, int_cmp_max));
    }

    assert(int_gdsp_heap_size(heap) == 0);

    int_gdsp_heap_destroy(heap);

    print_test_ok(name);
}

/* =========================================================
   TEST 10 : HEAP_SORT
   ========================================================= */

static void test_heap_sort(void)
{
    const char *name = "heap_sort";
    print_test_start(name);

    int values[] = {
        10, -5, 0, 8, 8, 3, 100, -50, 7, 1, 2, 999, -999
    };

    size_t n = sizeof(values) / sizeof(values[0]);

    int expected[sizeof(values) / sizeof(values[0])];

    for (size_t i = 0; i < n; i++)
        expected[i] = values[i];

    sort_int_array_asc(expected, n);

    int_gdsp_vector *vec = int_gdsp_vector_create(NULL);
    assert(vec != NULL);

    for (size_t i = 0; i < n; i++)
        assert(int_gdsp_vector_push_back(vec, values[i]));

    int_gdsp_vector *sorted = int_gdsp_heap_sort(vec, int_cmp_max);
    assert(sorted != NULL);
    assert(sorted == vec);

    assert(int_gdsp_vector_size(vec) == n);
    assert(vector_is_sorted_asc(vec));

    for (size_t i = 0; i < n; i++) {
        int *x = int_gdsp_vector_get_at(vec, i);

        assert(x != NULL);

        if (*x != expected[i]) {
            fprintf(stderr,
                    "heap_sort mismatch at index %zu: got %d, expected %d\n",
                    i,
                    *x,
                    expected[i]);
            assert(false);
        }
    }

    int_gdsp_vector_destroy(vec);

    print_test_ok(name);
}

/* =========================================================
   TEST 11 : HEAP_SORT AVEC VECTOR VIDE ET SINGLETON
   ========================================================= */

static void test_heap_sort_edge_cases(void)
{
    const char *name = "heap_sort edge cases";
    print_test_start(name);

    {
        int_gdsp_vector *vec = int_gdsp_vector_create(NULL);
        assert(vec != NULL);

        int_gdsp_vector *sorted = int_gdsp_heap_sort(vec, int_cmp_max);

        assert(sorted == vec);
        assert(int_gdsp_vector_size(vec) == 0);

        int_gdsp_vector_destroy(vec);
    }

    {
        int_gdsp_vector *vec = int_gdsp_vector_create(NULL);
        assert(vec != NULL);

        assert(int_gdsp_vector_push_back(vec, 42));

        int_gdsp_vector *sorted = int_gdsp_heap_sort(vec, int_cmp_max);

        assert(sorted == vec);
        assert(int_gdsp_vector_size(vec) == 1);
        assert(*int_gdsp_vector_get_at(vec, 0) == 42);

        int_gdsp_vector_destroy(vec);
    }

    print_test_ok(name);
}

/* =========================================================
   TEST 12 : VECTORIZE / RELEASE DU VECTOR INTERNE
   ========================================================= */

static void test_heap_vectorize(void)
{
    const char *name = "heap_vectorize";
    print_test_start(name);

    int_gdsp_heap *heap = int_gdsp_heap_create(int_cmp_max, NULL);
    assert(heap != NULL);

    assert(int_gdsp_heap_push(heap, 10));
    assert(int_gdsp_heap_push(heap, 50));
    assert(int_gdsp_heap_push(heap, 20));

    assert(check_heap_invariant_heap(heap, int_cmp_max));

    int_gdsp_vector *vec = int_gdsp_heap_vectorize(heap);

    /*
        Après cette ligne :
        - heap ne doit plus être utilisée.
        - vec doit être détruit manuellement.
    */

    assert(vec != NULL);
    assert(int_gdsp_vector_size(vec) == 3);
    assert(check_heap_invariant_vector(vec, int_cmp_max));

    int_gdsp_vector_destroy(vec);

    print_test_ok(name);
}

/* =========================================================
   TEST 13 : CLEAR
   ========================================================= */

static void test_clear(void)
{
    const char *name = "clear";
    print_test_start(name);

    int_gdsp_heap *heap = int_gdsp_heap_create(int_cmp_max, NULL);
    assert(heap != NULL);

    for (int i = 0; i < 100; i++)
        assert(int_gdsp_heap_push(heap, i));

    assert(int_gdsp_heap_size(heap) == 100);
    assert(int_gdsp_heap_peek(heap) != NULL);

    int_gdsp_heap_clear(heap);

    assert(int_gdsp_heap_size(heap) == 0);
    assert(int_gdsp_heap_peek(heap) == NULL);

    int out = 0;
    assert(int_gdsp_heap_pop(heap, &out) == false);

    int_gdsp_heap_destroy(heap);

    print_test_ok(name);
}

/* =========================================================
   TEST 14 : DESTROY_DATA AVEC CLEAR / DESTROY
   ========================================================= */

static void test_destroy_data(void)
{
    const char *name = "destroy_data";
    print_test_start(name);

    g_destroy_count = 0;

    int_gdsp_heap *heap = int_gdsp_heap_create(int_cmp_max, int_destroy);
    assert(heap != NULL);

    for (int i = 0; i < 10; i++)
        assert(int_gdsp_heap_push(heap, i));

    assert(g_destroy_count == 0);

    int_gdsp_heap_clear(heap);

    /*
        clear doit avoir appelé destroy_data une fois par élément.
    */
    assert(g_destroy_count == 10);
    assert(int_gdsp_heap_size(heap) == 0);

    for (int i = 0; i < 5; i++)
        assert(int_gdsp_heap_push(heap, i));

    int_gdsp_heap_destroy(heap);

    /*
        destroy doit appeler clear, donc +5.
    */
    assert(g_destroy_count == 15);

    print_test_ok(name);
}

/* =========================================================
   TEST 15 : POP AVEC OUT_DATA ET DESTROY_DATA
   ========================================================= */

static void test_pop_with_out_data_does_not_destroy_returned_value(void)
{
    const char *name = "pop with out_data and destroy_data";
    print_test_start(name);

    g_destroy_count = 0;

    int_gdsp_heap *heap = int_gdsp_heap_create(int_cmp_max, int_destroy);
    assert(heap != NULL);

    assert(int_gdsp_heap_push(heap, 10));
    assert(int_gdsp_heap_push(heap, 20));
    assert(int_gdsp_heap_push(heap, 30));

    int out = 0;

    assert(int_gdsp_heap_pop(heap, &out));
    assert(out == 30);

    /*
        Selon la convention normale :
        si out_data != NULL, l'élément extrait est transféré à l'appelant,
        donc destroy_data ne doit PAS être appelé sur cet élément.
    */
    assert(g_destroy_count == 0);

    int_gdsp_heap_destroy(heap);

    /*
        Il reste normalement 2 éléments dans la heap.
    */
    assert(g_destroy_count == 2);

    print_test_ok(name);
}

/* =========================================================
   MAIN
   ========================================================= */

int main(void)
{
    printf("========================================\n");
    printf(" GDSP HEAP TEST SUITE\n");
    printf("========================================\n\n");

    test_create_destroy();
    test_empty_heap();
    test_push_peek_simple();
    test_pop_order_max_heap();
    test_min_heap();
    test_duplicates();
    test_negative_values();
    test_random_stress();
    test_heapify();
    test_heap_sort();
    test_heap_sort_edge_cases();
    test_heap_vectorize();
    test_clear();
    test_destroy_data();
    test_pop_with_out_data_does_not_destroy_returned_value();

    printf("========================================\n");
    printf(" ALL TESTS PASSED ✅\n");
    printf("========================================\n");

    return 0;
}
