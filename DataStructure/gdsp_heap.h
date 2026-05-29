#ifndef GDSP_HEAP_H
#define GDSP_HEAP_H

#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>
#include "gdsp_vector.h"

/* =========================================================
   DECLARING THE HEAP (to use in a .h)
   ========================================================= */
#define DECLARE_GDSP_HEAP(TYPE, NAME, VEC_NAME)                         \
    DECLARE_GDSP_VECTOR(TYPE, VEC_NAME)                                 \
                                                                        \
    typedef struct {                                                    \
        VEC_NAME##_gdsp_vector *vec;                                    \
        int (*cmp)(TYPE, TYPE);                                         \
    }   NAME##_gdsp_heap;                                               \
                                                                        \
    NAME##_gdsp_heap *NAME##_gdsp_heap_create(int (*cmp)(TYPE, TYPE),   \
                                void (*destroy_data)(TYPE *));          \
                                                                        \
    void NAME##_gdsp_heap_clear(NAME##_gdsp_heap *heap);                \
    void NAME##_gdsp_heap_destroy(NAME##_gdsp_heap *heap);              \
                                                                        \
    bool NAME##_gdsp_heap_heapify_down(VEC_NAME##_gdsp_vector *vec,     \
                              size_t i,                                 \
                              size_t size,                              \
                              int (*cmp)(TYPE, TYPE));                  \
                                                                        \
    bool NAME##_gdsp_heap_push(NAME##_gdsp_heap *heap, TYPE val);       \
    TYPE *NAME##_gdsp_heap_peek(NAME##_gdsp_heap *heap);                \
    bool NAME##_gdsp_heap_pop(NAME##_gdsp_heap *heap, TYPE *out_data);  \
    size_t NAME##_gdsp_heap_size(NAME##_gdsp_heap *heap);               \
                                                                        \
    NAME##_gdsp_heap *NAME##_gdsp_heap_heapify(VEC_NAME##_gdsp_vector *vec, int (*cmp)(TYPE, TYPE));\
    VEC_NAME##_gdsp_vector *NAME##_gdsp_heap_vectorize(NAME##_gdsp_heap *heap);\
    VEC_NAME##_gdsp_vector *NAME##_gdsp_heap_sort(VEC_NAME##_gdsp_vector *vec, int (*cmp)(TYPE, TYPE));\

    /* =========================================================
   BUILDING THE HEAP (to use in a .c)
   ========================================================= */
#define BUILD_GDSP_HEAP(TYPE, NAME, VEC_NAME)                                   \
    BUILD_GDSP_VECTOR(TYPE, VEC_NAME)                                           \
    NAME##_gdsp_heap *NAME##_gdsp_heap_create(int (*cmp)(TYPE, TYPE),           \
        void (*destroy_data)(TYPE *)){                                          \
        if(!cmp)                                                                \
            return NULL;                                                        \
                                                                                \
        NAME##_gdsp_heap *heap = calloc(1, sizeof(NAME##_gdsp_heap));           \
        if(!heap)                                                               \
            return NULL;                                                        \
                                                                                \
        heap->vec = VEC_NAME##_gdsp_vector_create(destroy_data);                \
        if(!(heap->vec)){                                                       \
            free(heap);                                                         \
            return NULL;                                                        \
        }                                                                       \
                                                                                \
        heap->cmp = cmp;                                                        \
        return heap;                                                            \
    }                                                                           \
                                                                                \
    void NAME##_gdsp_heap_clear(NAME##_gdsp_heap *heap){                        \
        if(!heap) return;                                                       \
        VEC_NAME##_gdsp_vector_clear(heap->vec);                                \
    }                                                                           \
                                                                                \
    void NAME##_gdsp_heap_destroy(NAME##_gdsp_heap *heap){                      \
        if(!heap) return;                                                       \
        VEC_NAME##_gdsp_vector_destroy(heap->vec);                              \
        free(heap);                                                             \
    }                                                                           \
                                                                                \
    bool NAME##_gdsp_heap_heapify_down(VEC_NAME##_gdsp_vector *vec,             \
                              size_t i,                                         \
                              size_t size,                                      \
                              int (*cmp)(TYPE, TYPE)){                          \
        if (!vec || !cmp || i >= size)                                          \
            return false;                                                       \
                                                                                \
        size_t l = 2 * i + 1;                                                   \
        size_t r = 2 * i + 2;                                                   \
        size_t largest = i;                                                     \
                                                                                \
        if (l < size) {                                                         \
            TYPE *pl = VEC_NAME##_gdsp_vector_get_at(vec, l);                   \
            TYPE *pi = VEC_NAME##_gdsp_vector_get_at(vec, i);                   \
            if (cmp(*pl, *pi) > 0)                                              \
                largest = l;                                                    \
        }                                                                       \
                                                                                \
        if (r < size) {                                                         \
            TYPE *pr = VEC_NAME##_gdsp_vector_get_at(vec, r);                   \
            TYPE *plg = VEC_NAME##_gdsp_vector_get_at(vec, largest);            \
            if (cmp(*pr, *plg) > 0)                                             \
                largest = r;                                                    \
        }                                                                       \
                                                                                \
        if (largest != i) {                                                     \
            TYPE *pa = VEC_NAME##_gdsp_vector_get_at(vec, i);                   \
            TYPE *pb = VEC_NAME##_gdsp_vector_get_at(vec, largest);             \
            TYPE tmp = *pa;                                                     \
            *pa = *pb;                                                          \
            *pb = tmp;                                                          \
                                                                                \
            return NAME##_gdsp_heap_heapify_down(vec, largest, size, cmp);      \
        }                                                                       \
                                                                                \
        return true;                                                            \
    }                                                                           \
    bool NAME##_gdsp_heap_push(NAME##_gdsp_heap *heap, TYPE val){               \
        if (!heap || !heap->vec || !heap->cmp) return false;                    \
                                                                                \
        if (!VEC_NAME##_gdsp_vector_push_back(heap->vec, val))                  \
            return false;                                                       \
                                                                                \
        size_t i = VEC_NAME##_gdsp_vector_size(heap->vec) - 1;                  \
        size_t parent = (i == 0) ? 0 : (i - 1) / 2;                             \
        TYPE  *pi = VEC_NAME##_gdsp_vector_get_at(heap->vec, i);                \
        TYPE  *pp = VEC_NAME##_gdsp_vector_get_at(heap->vec, parent);           \
                                                                                \
        while (i > 0 && heap->cmp(*pi, *pp) > 0) {                              \
            TYPE tmp = *pi;                                                     \
            *pi = *pp;                                                          \
            *pp = tmp;                                                          \
                                                                                \
            i = parent;                                                         \
            parent = (i == 0) ? 0 : (i - 1) / 2;                                \
            pi = VEC_NAME##_gdsp_vector_get_at(heap->vec, i);                   \
            pp = VEC_NAME##_gdsp_vector_get_at(heap->vec, parent);              \
        }                                                                       \
                                                                                \
        return true;                                                            \
    }                                                                           \
    TYPE *NAME##_gdsp_heap_peek(NAME##_gdsp_heap *heap){                        \
        if(!heap || !heap->vec || heap->vec->size == 0)                         \
            return NULL;                                                        \
        return VEC_NAME##_gdsp_vector_front(heap->vec);                         \
    }                                                                           \
    bool NAME##_gdsp_heap_pop(NAME##_gdsp_heap *heap, TYPE *out_data)           \
    {                                                                           \
        if (!heap || !heap->vec || !heap->cmp)                                  \
            return false;                                                       \
                                                                                \
        size_t n = VEC_NAME##_gdsp_vector_size(heap->vec);                      \
                                                                                \
        if (n == 0)                                                             \
            return false;                                                       \
                                                                                \
        TYPE *root = VEC_NAME##_gdsp_vector_get_at(heap->vec, 0);               \
                                                                                \
        if (!root)                                                              \
            return false;                                                       \
                                                                                \
        if (out_data)                                                           \
            *out_data = *root;                                                  \
                                                                                \
        if (n == 1) {                                                           \
                                                                                \
            return VEC_NAME##_gdsp_vector_pop_back(heap->vec, NULL);            \
        }                                                                       \
                                                                                \
        TYPE last;                                                              \
                                                                                \
        if (!VEC_NAME##_gdsp_vector_pop_back(heap->vec, &last))                 \
            return false;                                                       \
                                                                                \
        if (!out_data && heap->vec->destroy_data)                               \
            heap->vec->destroy_data(root);                                      \
                                                                                \
        *root = last;                                                           \
                                                                                \
        return NAME##_gdsp_heap_heapify_down(                                   \
            heap->vec,                                                          \
            0,                                                                  \
            VEC_NAME##_gdsp_vector_size(heap->vec),                             \
            heap->cmp                                                           \
        );                                                                      \
    }                                                                           \
    size_t NAME##_gdsp_heap_size(NAME##_gdsp_heap *heap){                       \
        return heap->vec->size;                                                 \
    }                                                                           \
    NAME##_gdsp_heap *NAME##_gdsp_heap_heapify(                                 \
        VEC_NAME##_gdsp_vector *vec,                                            \
        int (*cmp)(TYPE, TYPE))                                                 \
    {                                                                           \
        if (!vec || !cmp)                                                       \
            return NULL;                                                        \
                                                                                \
        NAME##_gdsp_heap *heap = malloc(sizeof(*heap));                         \
                                                                                \
        if (!heap)                                                              \
            return NULL;                                                        \
                                                                                \
        heap->vec = vec;                                                        \
        heap->cmp = cmp;                                                        \
                                                                                \
        size_t n = VEC_NAME##_gdsp_vector_size(vec);                            \
                                                                                \
        for (size_t i = n / 2; i > 0; i--) {                                    \
            if (!NAME##_gdsp_heap_heapify_down(vec, i - 1, n, cmp)) {           \
                free(heap);                                                     \
                return NULL;                                                    \
            }                                                                   \
        }                                                                       \
                                                                                \
        return heap;                                                            \
    }                                                                           \
    VEC_NAME##_gdsp_vector *NAME##_gdsp_heap_vectorize(NAME##_gdsp_heap *heap){ \
        VEC_NAME##_gdsp_vector *vec = heap->vec;                                \
        free(heap);                                                             \
        return vec;                                                             \
    }                                                                           \
    VEC_NAME##_gdsp_vector *NAME##_gdsp_heap_sort(                              \
    VEC_NAME##_gdsp_vector *vec,                                                \
    int (*cmp)(TYPE, TYPE))                                                     \
    {                                                                           \
        if (!vec || !cmp)                                                       \
            return NULL;                                                        \
                                                                                \
        size_t n = VEC_NAME##_gdsp_vector_size(vec);                            \
                                                                                \
        if (n < 2)                                                              \
            return vec;                                                         \
                                                                                \
        for (size_t i = n / 2; i > 0; i--) {                                    \
            if (!NAME##_gdsp_heap_heapify_down(vec, i - 1, n, cmp))             \
                return NULL;                                                    \
        }                                                                       \
                                                                                \
        for (size_t heap_size = n; heap_size > 1; heap_size--) {                \
            TYPE *a = VEC_NAME##_gdsp_vector_get_at(vec, 0);                    \
            TYPE *b = VEC_NAME##_gdsp_vector_get_at(vec, heap_size - 1);        \
                                                                                \
            if (!a || !b)                                                       \
                return NULL;                                                    \
                                                                                \
            TYPE tmp = *a;                                                      \
            *a = *b;                                                            \
            *b = tmp;                                                           \
                                                                                \
            if (!NAME##_gdsp_heap_heapify_down(vec, 0, heap_size - 1, cmp))     \
                return NULL;                                                    \
        }                                                                       \
                                                                                \
        return vec;                                                             \
    }                                                                           \


#endif /* GDSP_HEAP_H */