#ifndef GDSP_VECTOR_H
#define GDSP_VECTOR_H

#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>

/* =========================================================
   DECLARING THE VECTOR (to use in a .h)
   ========================================================= */
#define DECLARE_GDSP_VECTOR(TYPE, NAME)                             \
typedef struct NAME##_gdsp_vector {                                 \
    TYPE *datas;                                                    \
    size_t capacity;                                                \
    size_t size;                                                    \
    void (*destroy_data)(TYPE *);                                   \
} NAME##_gdsp_vector;                                               \
                                                                    \
NAME##_gdsp_vector *NAME##_gdsp_vector_create(                      \
    void (*destroy_data)(TYPE *));                                  \
                                                                    \
void NAME##_gdsp_vector_destroy(NAME##_gdsp_vector *vector);        \
void NAME##_gdsp_vector_clear(NAME##_gdsp_vector *vector);          \
                                                                    \
bool NAME##_gdsp_vector_empty(const NAME##_gdsp_vector *vector);    \
size_t NAME##_gdsp_vector_size(const NAME##_gdsp_vector *vector);   \
                                                                    \
bool NAME##_gdsp_vector_push_front(NAME##_gdsp_vector *vector,      \
                                    TYPE data);                     \
bool NAME##_gdsp_vector_push_back(NAME##_gdsp_vector *vector,       \
                                    TYPE data);                     \
bool NAME##_gdsp_vector_insert_at(NAME##_gdsp_vector *vector,       \
                                    TYPE data,                      \
                                    size_t index);                  \
                                                                    \
TYPE *NAME##_gdsp_vector_front(NAME##_gdsp_vector *vector);         \
TYPE *NAME##_gdsp_vector_back(NAME##_gdsp_vector *vector);          \
TYPE *NAME##_gdsp_vector_get_at(NAME##_gdsp_vector *vector,         \
                                size_t index);                      \
                                                                    \
bool NAME##_gdsp_vector_pop_front(NAME##_gdsp_vector *vector,       \
                                    TYPE *out_data);                \
bool NAME##_gdsp_vector_pop_back(NAME##_gdsp_vector *vector,        \
                                    TYPE *out_data);                \
bool NAME##_gdsp_vector_pop_at(NAME##_gdsp_vector *vector,          \
                                    size_t index,                   \
                                    TYPE *out_data);                \
                                                                    \
bool NAME##_gdsp_vector_resize(NAME##_gdsp_vector *vector);




#endif /* GDSP_VECTOR_H */