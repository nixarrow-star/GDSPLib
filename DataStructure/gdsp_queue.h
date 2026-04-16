#ifndef GSDP_QUEUE_H
#define GSDP_QUEUE_H

#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>

#define DECLARE_GDSP_QUEUE(TYPE, NAME)                                            \
                                                                                  \
typedef struct NAME##_gdsp_queue_node {                                           \
    TYPE data;                                                                    \
    struct NAME##_gdsp_queue_node *next;                                          \
} NAME##_gdsp_queue_node;                                                         \
                                                                                  \
typedef struct NAME##_gdsp_queue {                                                \
    NAME##_gdsp_queue_node *head;                                                 \
    NAME##_gdsp_queue_node *tail;                                                 \
    size_t size;                                                                  \
    void (*destroy_data)(TYPE *);                                                 \
} NAME##_gdsp_queue;                                                              \
                                                                                  \
NAME##_gdsp_queue *NAME##_gdsp_queue_create(                                      \
    void (*destroy_data)(TYPE *));                                                \
                                                                                  \
void NAME##_gdsp_queue_destroy(NAME##_gdsp_queue *list);                          \
void NAME##_gdsp_queue_clear(NAME##_gdsp_queue *list);                            \
                                                                                  \
bool NAME##_gdsp_queue_empty(const NAME##_gdsp_queue *list);                      \
size_t NAME##_gdsp_queue_size(const NAME##_gdsp_queue *list);                     \
                                                                                  \
bool NAME##_gdsp_queue_push(NAME##_gdsp_queue *list, TYPE data);                  \
TYPE *NAME##_gdsp_queue_peek(NAME##_gdsp_queue *list);                            \
bool NAME##_gdsp_queue_pop(NAME##_gdsp_queue *list, TYPE *out_data);

#define BUILD_GDSP_QUEUE(TYPE, NAME)                                              \
                                                                                  \
NAME##_gdsp_queue *NAME##_gdsp_queue_create(void (*destroy_data)(TYPE *))         \
{                                                                                 \
    NAME##_gdsp_queue *list = calloc(1, sizeof(NAME##_gdsp_queue));               \
    if (!list)                                                                    \
        return NULL;                                                              \
    list->destroy_data = destroy_data;                                            \
    return list;                                                                  \
}                                                                                 \
                                                                                  \
void NAME##_gdsp_queue_clear(NAME##_gdsp_queue *list)                             \
{                                                                                 \
    if (!list)                                                                    \
        return;                                                                   \
                                                                                  \
    NAME##_gdsp_queue_node *current = list->head;                                 \
    while (current)                                                               \
    {                                                                             \
        NAME##_gdsp_queue_node *next = current->next;                             \
        if (list->destroy_data)                                                   \
            list->destroy_data(&current->data);                                   \
        free(current);                                                            \
        current = next;                                                           \
    }                                                                             \
                                                                                  \
    list->head = NULL;                                                            \
    list->tail = NULL;                                                            \
    list->size = 0;                                                               \
}                                                                                 \
                                                                                  \
void NAME##_gdsp_queue_destroy(NAME##_gdsp_queue *list)                           \
{                                                                                 \
    if (!list)                                                                    \
        return;                                                                   \
    NAME##_gdsp_queue_clear(list);                                                \
    free(list);                                                                   \
}                                                                                 \
                                                                                  \
bool NAME##_gdsp_queue_empty(const NAME##_gdsp_queue *list)                       \
{                                                                                 \
    if (!list)                                                                    \
        return true;                                                              \
    return list->size == 0;                                                       \
}                                                                                 \
                                                                                  \
size_t NAME##_gdsp_queue_size(const NAME##_gdsp_queue *list)                      \
{                                                                                 \
    if (!list)                                                                    \
        return 0;                                                                 \
    return list->size;                                                            \
}                                                                                 \
                                                                                  \
bool NAME##_gdsp_queue_push(NAME##_gdsp_queue *list, TYPE data)                   \
{                                                                                 \
    if (!list)                                                                    \
        return false;                                                             \
                                                                                  \
    NAME##_gdsp_queue_node *node = calloc(1, sizeof(NAME##_gdsp_queue_node));     \
    if (!node)                                                                    \
        return false;                                                             \
                                                                                  \
    node->data = data;                                                            \
    node->next = list->head;                                                      \
    list->head = node;                                                            \
                                                                                  \
    if (list->size == 0)                                                          \
        list->tail = node;                                                        \
                                                                                  \
    list->size++;                                                                 \
    return true;                                                                  \
}                                                                                 \
                                                                                  \
TYPE *NAME##_gdsp_queue_peek(NAME##_gdsp_queue *list)                             \
{                                                                                 \
    if (!list || list->size == 0)                                                 \
        return NULL;                                                              \
    return &list->tail->data;                                                     \
}                                                                                 \
                                                                                  \
bool NAME##_gdsp_queue_pop(NAME##_gdsp_queue *list, TYPE *out_data)               \
{                                                                                 \
    if (!list || list->size == 0)                                                 \
        return false;                                                             \
                                                                                  \
    if (list->size == 1)                                                          \
    {                                                                             \
        NAME##_gdsp_queue_node *node = list->tail;                                \
        if (out_data)                                                             \
            *out_data = node->data;                                               \
        list->head = NULL;                                                        \
        list->tail = NULL;                                                        \
        list->size = 0;                                                           \
        free(node);                                                               \
        return true;                                                              \
    }                                                                             \
                                                                                  \
    NAME##_gdsp_queue_node *previous = list->head;                                \
    while (previous->next != list->tail)                                          \
        previous = previous->next;                                                \
                                                                                  \
    NAME##_gdsp_queue_node *node = list->tail;                                    \
    if (out_data)                                                                 \
        *out_data = node->data;                                                   \
                                                                                  \
    previous->next = NULL;                                                        \
    list->tail = previous;                                                        \
    list->size--;                                                                 \
                                                                                  \
    free(node);                                                                   \
    return true;                                                                  \
}

#endif /* GSDP_QUEUE_H */