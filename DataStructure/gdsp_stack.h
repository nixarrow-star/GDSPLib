#ifndef GDSP_STACK_H
#define GDSP_STACK_H

#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>

#define DECLARE_GDSP_STACK(TYPE, NAME)                                            \
                                                                                  \
typedef struct NAME##_gdsp_stack_node {                                           \
    TYPE data;                                                                    \
    struct NAME##_gdsp_stack_node *next;                                          \
} NAME##_gdsp_stack_node;                                                         \
                                                                                  \
typedef struct NAME##_gdsp_stack {                                                \
    NAME##_gdsp_stack_node *head;                                                 \
    size_t size;                                                                  \
    void (*destroy_data)(TYPE *);                                                 \
} NAME##_gdsp_stack;                                                              \
                                                                                  \
NAME##_gdsp_stack *NAME##_gdsp_stack_create(                                      \
    void (*destroy_data)(TYPE *));                                                \
                                                                                  \
void NAME##_gdsp_stack_destroy(NAME##_gdsp_stack *list);                          \
void NAME##_gdsp_stack_clear(NAME##_gdsp_stack *list);                            \
                                                                                  \
bool NAME##_gdsp_stack_empty(const NAME##_gdsp_stack *list);                      \
size_t NAME##_gdsp_stack_size(const NAME##_gdsp_stack *list);                     \
                                                                                  \
bool NAME##_gdsp_stack_push(NAME##_gdsp_stack *list, TYPE data);                  \
TYPE *NAME##_gdsp_stack_peek(NAME##_gdsp_stack *list);                            \
bool NAME##_gdsp_stack_pop(NAME##_gdsp_stack *list, TYPE *out_data);

#define BUILD_GDSP_STACK(TYPE, NAME)                                              \
                                                                                  \
NAME##_gdsp_stack *NAME##_gdsp_stack_create(void (*destroy_data)(TYPE *))         \
{                                                                                 \
    NAME##_gdsp_stack *list = calloc(1, sizeof(NAME##_gdsp_stack));               \
    if (!list)                                                                    \
        return NULL;                                                              \
    list->destroy_data = destroy_data;                                            \
    return list;                                                                  \
}                                                                                 \
                                                                                  \
void NAME##_gdsp_stack_clear(NAME##_gdsp_stack *list)                             \
{                                                                                 \
    if (!list)                                                                    \
        return;                                                                   \
                                                                                  \
    NAME##_gdsp_stack_node *current = list->head;                                 \
    while (current)                                                               \
    {                                                                             \
        NAME##_gdsp_stack_node *next = current->next;                             \
        if (list->destroy_data)                                                   \
            list->destroy_data(&current->data);                                   \
        free(current);                                                            \
        current = next;                                                           \
    }                                                                             \
                                                                                  \
    list->head = NULL;                                                            \
    list->size = 0;                                                               \
}                                                                                 \
                                                                                  \
void NAME##_gdsp_stack_destroy(NAME##_gdsp_stack *list)                           \
{                                                                                 \
    if (!list)                                                                    \
        return;                                                                   \
    NAME##_gdsp_stack_clear(list);                                                \
    free(list);                                                                   \
}                                                                                 \
                                                                                  \
bool NAME##_gdsp_stack_empty(const NAME##_gdsp_stack *list)                       \
{                                                                                 \
    if (!list)                                                                    \
        return true;                                                              \
    return list->size == 0;                                                       \
}                                                                                 \
                                                                                  \
size_t NAME##_gdsp_stack_size(const NAME##_gdsp_stack *list)                      \
{                                                                                 \
    if (!list)                                                                    \
        return 0;                                                                 \
    return list->size;                                                            \
}                                                                                 \
                                                                                  \
bool NAME##_gdsp_stack_push(NAME##_gdsp_stack *list, TYPE data)                   \
{                                                                                 \
    if (!list)                                                                    \
        return false;                                                             \
                                                                                  \
    NAME##_gdsp_stack_node *node = calloc(1, sizeof(NAME##_gdsp_stack_node));     \
    if (!node)                                                                    \
        return false;                                                             \
                                                                                  \
    node->data = data;                                                            \
    node->next = list->head;                                                      \
    list->head = node;                                                            \
    list->size++;                                                                 \
    return true;                                                                  \
}                                                                                 \
                                                                                  \
TYPE *NAME##_gdsp_stack_peek(NAME##_gdsp_stack *list)                             \
{                                                                                 \
    if (!list || list->size == 0)                                                 \
        return NULL;                                                              \
    return &list->head->data;                                                     \
}                                                                                 \
                                                                                  \
bool NAME##_gdsp_stack_pop(NAME##_gdsp_stack *list, TYPE *out_data)               \
{                                                                                 \
    if (!list || list->size == 0)                                                 \
        return false;                                                             \
                                                                                  \
    NAME##_gdsp_stack_node *node = list->head;                                    \
    if (out_data)                                                                 \
        *out_data = node->data;                                                   \
                                                                                  \
    list->head = node->next;                                                      \
    list->size--;                                                                 \
    free(node);                                                                   \
    return true;                                                                  \
}

#endif /* GDSP_STACK_H */