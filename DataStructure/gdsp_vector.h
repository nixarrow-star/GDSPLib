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
bool NAME##_gdsp_vector_reserve(NAME##_gdsp_vector *vector,    \
    size_t n_capacity);                                             \
                                                                    \
bool NAME##_gdsp_vector_empty(const NAME##_gdsp_vector *vector);    \
size_t NAME##_gdsp_vector_capacity(const NAME##_gdsp_vector *vector);\
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
                                    TYPE *out_data);

/* =========================================================
   BUILDING THE VECTOR (to use in a .c)
   ========================================================= */
#define BUILDING_GDSP_VECTOR(TYPE, NAME)                                                            \
                                                                                                    \
NAME##_gdsp_vector *NAME##_gdsp_vector_create(                                                      \
    void (*destroy_data)(TYPE *)) {                                                                 \
        NAME##_gdsp_vector *vector = calloc(1, sizeof(NAME##_gdsp_vector));                         \
        if(!vector)                                                                                 \
            return NULL;                                                                            \
                                                                                                    \
        vector->destroy_data = destroy_data;                                                        \
        vector->size = 0;                                                                           \
        vector-> capacity = 8;                                                                      \
                                                                                                    \
        TYPE *data = calloc(8, sizeof(TYPE));                                                       \
        if(!data){                                                                                  \
            free(vector);                                                                           \
            return NULL;                                                                            \
        }                                                                                           \
        vector->datas = data;                                                                       \
        return vector;                                                                              \
    }                                                                                               \
                                                                                                    \
void NAME##_gdsp_vector_clear(NAME##_gdsp_vector *vector) {                                         \
    if(!vector)                                                                                     \
        return;                                                                                     \
                                                                                                    \
    if(vector->destroy_data){                                                                       \
        size_t i = 0;                                                                               \
        while(i < vector->size){                                                                    \
            vector->destroy_data(&vector->datas[i]);                                                \
            i++;                                                                                    \
        }                                                                                           \
    }                                                                                               \
    vector->size = 0;                                                                               \
}                                                                                                   \
                                                                                                    \
void NAME##_gdsp_vector_destroy(NAME##_gdsp_vector *vector){                                        \
    if(!vector)                                                                                     \
        return;                                                                                     \
                                                                                                    \
    NAME##_gdsp_vector_clear(vector);                                                               \
    free(vector->datas);                                                                            \
    free(vector);                                                                                   \
}                                                                                                   \
                                                                                                    \
bool NAME##_gdsp_vector_reserve(NAME##_gdsp_vector *vector, size_t n_capacity){                     \
    if(!vector)                                                                                     \
        return false;                                                                               \
                                                                                                    \
    if(n_capacity <= vector->capacity)                                                              \
        return true;                                                                                \
                                                                                                    \
    if(n_capacity < vector->size)                                                                   \
        n_capacity = vector->size;                                                                  \
                                                                                                    \
    TYPE *new_datas = realloc(vector->datas, sizeof(TYPE) * n_capacity);                            \
    if(!new_datas)                                                                                  \
        return false;                                                                               \
                                                                                                    \
    vector->datas = new_datas;                                                                      \
    vector->capacity = n_capacity;                                                                  \
    return true;                                                                                    \
}                                                                                                   \
                                                                                                    \
bool NAME##_gdsp_vector_empty(const NAME##_gdsp_vector *vector){                                    \
    if(!vector)                                                                                     \
        return false;                                                                               \
                                                                                                    \
    return vector->size == 0 ? true : false;                                                        \
}                                                                                                   \
                                                                                                    \
size_t NAME##_gdsp_vector_capacity(const NAME##_gdsp_vector *vector){                               \
    if(!vector)                                                                                     \
        return 0;                                                                                   \
                                                                                                    \
    return vector->capacity;                                                                        \
}                                                                                                   \
                                                                                                    \
size_t NAME##_gdsp_vector_size(const NAME##_gdsp_vector *vector){                                   \
    if(!vector)                                                                                     \
        return 0;                                                                                   \
                                                                                                    \
    return vector->size;                                                                            \
}                                                                                                   \
                                                                                                    \
bool NAME##_gdsp_vector_push_front(NAME##_gdsp_vector *vector, TYPE data)                           \
{                                                                                                   \
    if (!vector)                                                                                    \
        return false;                                                                               \
                                                                                                    \
    if (vector->size == vector->capacity &&                                                         \
        !NAME##_gdsp_vector_reserve(vector, vector->capacity * 2 + 1))                              \
        return false;                                                                               \
                                                                                                    \
    for (size_t i = vector->size; i > 0; i--)                                                       \
        vector->datas[i] = vector->datas[i - 1];                                                    \
                                                                                                    \
    vector->datas[0] = data;                                                                        \
    vector->size += 1;                                                                              \
    return true;                                                                                    \
}                                                                                                   \
bool NAME##_gdsp_vector_push_back(NAME##_gdsp_vector *vector, TYPE data)                            \
{                                                                                                   \
    if (!vector)                                                                                    \
        return false;                                                                               \
                                                                                                    \
    if (vector->size == vector->capacity &&                                                         \
        !NAME##_gdsp_vector_reserve(vector, vector->capacity * 2 + 1))                              \
        return false;                                                                               \
                                                                                                    \
    vector->datas[vector->size] = data;                                                             \
    vector->size += 1;                                                                              \
    return true;                                                                                    \
}                                                                                                   \
                                                                                                    \
bool NAME##_gdsp_vector_insert_at(NAME##_gdsp_vector *vector,                                       \
                                   TYPE data, size_t index)                                         \
{                                                                                                   \
    if (!vector)                                                                                    \
        return false;                                                                               \
                                                                                                    \
    if (index > vector->size)                                                                       \
        return false;                                                                               \
                                                                                                    \
    if (index == 0)                                                                                 \
        return NAME##_gdsp_vector_push_front(vector, data);                                         \
                                                                                                    \
    if (index == vector->size)                                                                      \
        return NAME##_gdsp_vector_push_back(vector, data);                                          \
                                                                                                    \
    if (vector->size == vector->capacity &&                                                         \
        !NAME##_gdsp_vector_reserve(vector, vector->capacity * 2 + 1))                              \
        return false;                                                                               \
                                                                                                    \
    for (size_t i = vector->size; i > index; i--)                                                   \
        vector->datas[i] = vector->datas[i - 1];                                                    \
                                                                                                    \
    vector->datas[index] = data;                                                                    \
    vector->size += 1;                                                                              \
    return true;                                                                                    \
}                                                                                                   \
                                                                                                    \
TYPE *NAME##_gdsp_vector_front(NAME##_gdsp_vector *vector)                                          \
{                                                                                                   \
    if (!vector || vector->size == 0)                                                               \
        return NULL;                                                                                \
                                                                                                    \
    return &vector->datas[0];                                                                       \
}                                                                                                   \
                                                                                                    \
TYPE *NAME##_gdsp_vector_back(NAME##_gdsp_vector *vector)                                           \
{                                                                                                   \
    if (!vector || vector->size == 0)                                                               \
        return NULL;                                                                                \
                                                                                                    \
    return &vector->datas[vector->size - 1];                                                        \
}                                                                                                   \
                                                                                                    \
TYPE *NAME##_gdsp_vector_get_at(NAME##_gdsp_vector *vector, size_t index)                           \
{                                                                                                   \
    if (!vector || vector->size == 0)                                                               \
        return NULL;                                                                                \
                                                                                                    \
    if (index >= vector->size)                                                                      \
        return NULL;                                                                                \
                                                                                                    \
    return &vector->datas[index];                                                                   \
}                                                                                                   \
                                                                                                    \
bool NAME##_gdsp_vector_pop_front(NAME##_gdsp_vector *vector,                                       \
                                   TYPE *out_data)                                                  \
{                                                                                                   \
    if (!vector || vector->size == 0)                                                               \
        return false;                                                                               \
                                                                                                    \
    if (out_data)                                                                                   \
        *out_data = vector->datas[0];                                                               \
    else if (vector->destroy_data)                                                                  \
        vector->destroy_data(&vector->datas[0]);                                                    \
                                                                                                    \
    for (size_t i = 0; i < vector->size - 1; i++)                                                   \
        vector->datas[i] = vector->datas[i + 1];                                                    \
                                                                                                    \
    vector->size -= 1;                                                                              \
    return true;                                                                                    \
}                                                                                                   \
                                                                                                    \
bool NAME##_gdsp_vector_pop_back(NAME##_gdsp_vector *vector,                                        \
                                  TYPE *out_data)                                                   \
{                                                                                                   \
    if (!vector || vector->size == 0)                                                               \
        return false;                                                                               \
                                                                                                    \
    if (out_data)                                                                                   \
        *out_data = vector->datas[vector->size - 1];                                                \
    else if (vector->destroy_data)                                                                  \
        vector->destroy_data(&vector->datas[vector->size - 1]);                                     \
                                                                                                    \
    vector->size -= 1;                                                                              \
    return true;                                                                                    \
}                                                                                                   \
                                                                                                    \
bool NAME##_gdsp_vector_pop_at(NAME##_gdsp_vector *vector,                                          \
                                size_t index, TYPE *out_data)                                       \
{                                                                                                   \
    if (!vector || vector->size == 0)                                                               \
        return false;                                                                               \
                                                                                                    \
    if (index >= vector->size)                                                                      \
        return false;                                                                               \
                                                                                                    \
    if (index == 0)                                                                                 \
        return NAME##_gdsp_vector_pop_front(vector, out_data);                                      \
                                                                                                    \
    if (index == vector->size - 1)                                                                  \
        return NAME##_gdsp_vector_pop_back(vector, out_data);                                       \
                                                                                                    \
    if (out_data)                                                                                   \
        *out_data = vector->datas[index];                                                           \
    else if (vector->destroy_data)                                                                  \
        vector->destroy_data(&vector->datas[index]);                                                \
                                                                                                    \
    for (size_t i = index; i < vector->size - 1; i++)                                               \
        vector->datas[i] = vector->datas[i + 1];                                                    \
                                                                                                    \
    vector->size -= 1;                                                                              \
    return true;                                                                                    \
}


#endif /* GDSP_VECTOR_H */