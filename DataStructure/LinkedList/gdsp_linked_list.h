#ifndef GDSP_LINKED_LIST_H
#define GDSP_LINKED_LIST_H

#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>

/* =========================================================
   DECLARING THE LINKED LIST (to use in a .h)
   ========================================================= */
#define DECLARE_GDSP_LINKED_LIST(TYPE, NAME)                                      \
                                                                                  \
typedef struct NAME##_gdsp_linked_list_node {                                     \
    TYPE data;                                                                    \
    struct NAME##_gdsp_linked_list_node *next;                                    \
} NAME##_gdsp_linked_list_node;                                                   \
                                                                                  \
typedef struct NAME##_gdsp_linked_list {                                          \
    NAME##_gdsp_linked_list_node *head;                                           \
    NAME##_gdsp_linked_list_node *tail;                                           \
    size_t size;                                                                  \
    void (*destroy_data)(TYPE *);                                                 \
} NAME##_gdsp_linked_list;                                                        \
                                                                                  \
NAME##_gdsp_linked_list *NAME##_gdsp_linked_list_create(                          \
    void (*destroy_data)(TYPE *));                                                \
                                                                                  \
void NAME##_gdsp_linked_list_destroy(NAME##_gdsp_linked_list *list);              \
void NAME##_gdsp_linked_list_clear(NAME##_gdsp_linked_list *list);                \
                                                                                  \
bool NAME##_gdsp_linked_list_empty(const NAME##_gdsp_linked_list *list);          \
size_t NAME##_gdsp_linked_list_size(const NAME##_gdsp_linked_list *list);         \
                                                                                  \
bool NAME##_gdsp_linked_list_push_front(NAME##_gdsp_linked_list *list,            \
                                        TYPE data);                               \
bool NAME##_gdsp_linked_list_push_back(NAME##_gdsp_linked_list *list,             \
                                       TYPE data);                                \
bool NAME##_gdsp_linked_list_insert_at(NAME##_gdsp_linked_list *list,             \
                                       TYPE data,                                 \
                                       size_t index);                             \
                                                                                  \
TYPE *NAME##_gdsp_linked_list_front(NAME##_gdsp_linked_list *list);               \
TYPE *NAME##_gdsp_linked_list_back(NAME##_gdsp_linked_list *list);                \
TYPE *NAME##_gdsp_linked_list_get_at(NAME##_gdsp_linked_list *list,               \
                                     size_t index);                               \
                                                                                  \
const TYPE *NAME##_gdsp_linked_list_front_const(                                  \
    const NAME##_gdsp_linked_list *list);                                         \
const TYPE *NAME##_gdsp_linked_list_back_const(                                   \
    const NAME##_gdsp_linked_list *list);                                         \
const TYPE *NAME##_gdsp_linked_list_get_at_const(                                 \
    const NAME##_gdsp_linked_list *list,                                          \
    size_t index);                                                                \
                                                                                  \
bool NAME##_gdsp_linked_list_pop_front(NAME##_gdsp_linked_list *list,             \
                                       TYPE *out_data);                           \
bool NAME##_gdsp_linked_list_pop_back(NAME##_gdsp_linked_list *list,              \
                                      TYPE *out_data);                            \
bool NAME##_gdsp_linked_list_pop_at(NAME##_gdsp_linked_list *list,                \
                                    size_t index,                                 \
                                    TYPE *out_data);                              \
                                                                                  \
bool NAME##_gdsp_linked_list_remove_at(NAME##_gdsp_linked_list *list,             \
                                       size_t index);

/* =========================================================
   DEFINING THE LINKED LIST (to use in .c)
   ========================================================= */
#define DEFINE_GDSP_LINKED_LIST(TYPE, NAME)                                                        \
                                                                                                   \
NAME##_gdsp_linked_list *NAME##_gdsp_linked_list_create(                                           \
    void (*destroy_data)(TYPE *)) {                                                                \
    NAME##_gdsp_linked_list *list = calloc(1, sizeof(NAME##_gdsp_linked_list));                    \
    if (!list) {                                                                                   \
        return NULL;                                                                               \
    }                                                                                              \
    list->destroy_data = destroy_data;                                                             \
    return list;                                                                                   \
}                                                                                                  \
                                                                                                   \
void NAME##_gdsp_linked_list_clear(NAME##_gdsp_linked_list *list) {                                \
    if (!list) {                                                                                   \
        return;                                                                                    \
    }                                                                                              \
                                                                                                   \
    NAME##_gdsp_linked_list_node *current = list->head;                                            \
    while (current) {                                                                              \
        NAME##_gdsp_linked_list_node *next = current->next;                                        \
        if (list->destroy_data) {                                                                  \
            list->destroy_data(&current->data);                                                    \
        }                                                                                          \
        free(current);                                                                             \
        current = next;                                                                            \
    }                                                                                              \
                                                                                                   \
    list->head = NULL;                                                                             \
    list->tail = NULL;                                                                             \
    list->size = 0;                                                                                \
}                                                                                                  \
                                                                                                   \
void NAME##_gdsp_linked_list_destroy(NAME##_gdsp_linked_list *list) {                              \
    if (!list) {                                                                                   \
        return;                                                                                    \
    }                                                                                              \
    NAME##_gdsp_linked_list_clear(list);                                                           \
    free(list);                                                                                    \
}                                                                                                  \
                                                                                                   \
bool NAME##_gdsp_linked_list_empty(const NAME##_gdsp_linked_list *list) {                          \
    if (!list) {                                                                                   \
        return true;                                                                               \
    }                                                                                              \
    return list->size == 0;                                                                        \
}                                                                                                  \
                                                                                                   \
size_t NAME##_gdsp_linked_list_size(const NAME##_gdsp_linked_list *list) {                         \
    if (!list) {                                                                                   \
        return 0;                                                                                  \
    }                                                                                              \
    return list->size;                                                                             \
}                                                                                                  \
                                                                                                   \
bool NAME##_gdsp_linked_list_push_front(NAME##_gdsp_linked_list *list,                             \
                                        TYPE data) {                                               \
    if (!list) {                                                                                   \
        return false;                                                                              \
    }                                                                                              \
                                                                                                   \
    NAME##_gdsp_linked_list_node *node = calloc(1, sizeof(NAME##_gdsp_linked_list_node));          \
    if (!node) {                                                                                   \
        return false;                                                                              \
    }                                                                                              \
                                                                                                   \
    node->data = data;                                                                             \
    node->next = list->head;                                                                       \
    list->head = node;                                                                             \
                                                                                                   \
    if (list->size == 0) {                                                                         \
        list->tail = node;                                                                         \
    }                                                                                              \
                                                                                                   \
    list->size++;                                                                                  \
    return true;                                                                                   \
}                                                                                                  \
                                                                                                   \
bool NAME##_gdsp_linked_list_push_back(NAME##_gdsp_linked_list *list,                              \
                                       TYPE data) {                                                \
    if (!list) {                                                                                   \
        return false;                                                                              \
    }                                                                                              \
                                                                                                   \
    if (list->size == 0) {                                                                         \
        return NAME##_gdsp_linked_list_push_front(list, data);                                     \
    }                                                                                              \
                                                                                                   \
    NAME##_gdsp_linked_list_node *node = calloc(1, sizeof(NAME##_gdsp_linked_list_node));          \
    if (!node) {                                                                                   \
        return false;                                                                              \
    }                                                                                              \
                                                                                                   \
    node->data = data;                                                                             \
    node->next = NULL;                                                                             \
                                                                                                   \
    list->tail->next = node;                                                                       \
    list->tail = node;                                                                             \
    list->size++;                                                                                  \
    return true;                                                                                   \
}                                                                                                  \
                                                                                                   \
bool NAME##_gdsp_linked_list_insert_at(NAME##_gdsp_linked_list *list,                              \
                                       TYPE data,                                                  \
                                       size_t index) {                                             \
    if (!list || index > list->size) {                                                             \
        return false;                                                                              \
    }                                                                                              \
                                                                                                   \
    if (index == 0) {                                                                              \
        return NAME##_gdsp_linked_list_push_front(list, data);                                     \
    }                                                                                              \
                                                                                                   \
    if (index == list->size) {                                                                     \
        return NAME##_gdsp_linked_list_push_back(list, data);                                      \
    }                                                                                              \
                                                                                                   \
    NAME##_gdsp_linked_list_node *node = calloc(1, sizeof(NAME##_gdsp_linked_list_node));          \
    if (!node) {                                                                                   \
        return false;                                                                              \
    }                                                                                              \
                                                                                                   \
    node->data = data;                                                            \
                                                                                  \
    NAME##_gdsp_linked_list_node *current = list->head;                           \
    while (index > 1) {                                                           \
        current = current->next;                                                  \
        index--;                                                                  \
    }                                                                             \
                                                                                  \
    node->next = current->next;                                                   \
    current->next = node;                                                         \
    list->size++;                                                                 \
    return true;                                                                  \
}                                                                                 \
                                                                                  \
TYPE *NAME##_gdsp_linked_list_front(NAME##_gdsp_linked_list *list) {              \
    if (!list || list->size == 0) {                                               \
        return NULL;                                                              \
    }                                                                             \
    return &list->head->data;                                                     \
}                                                                                 \
                                                                                  \
TYPE *NAME##_gdsp_linked_list_back(NAME##_gdsp_linked_list *list) {               \
    if (!list || list->size == 0) {                                               \
        return NULL;                                                              \
    }                                                                             \
    return &list->tail->data;                                                     \
}                                                                                 \
                                                                                  \
TYPE *NAME##_gdsp_linked_list_get_at(NAME##_gdsp_linked_list *list,               \
                                     size_t index) {                              \
    if (!list || index >= list->size) {                                           \
        return NULL;                                                              \
    }                                                                             \
                                                                                  \
    NAME##_gdsp_linked_list_node *current = list->head;                           \
    while (index > 0) {                                                           \
        current = current->next;                                                  \
        index--;                                                                  \
    }                                                                             \
    return &current->data;                                                        \
}                                                                                 \
                                                                                  \
const TYPE *NAME##_gdsp_linked_list_front_const(                                  \
    const NAME##_gdsp_linked_list *list) {                                        \
    if (!list || list->size == 0) {                                               \
        return NULL;                                                              \
    }                                                                             \
    return &list->head->data;                                                     \
}                                                                                 \
                                                                                  \
const TYPE *NAME##_gdsp_linked_list_back_const(                                   \
    const NAME##_gdsp_linked_list *list) {                                        \
    if (!list || list->size == 0) {                                               \
        return NULL;                                                              \
    }                                                                             \
    return &list->tail->data;                                                     \
}                                                                                 \
                                                                                  \
const TYPE *NAME##_gdsp_linked_list_get_at_const(                                 \
    const NAME##_gdsp_linked_list *list,                                          \
    size_t index) {                                                               \
    if (!list || index >= list->size) {                                           \
        return NULL;                                                              \
    }                                                                             \
                                                                                  \
    const NAME##_gdsp_linked_list_node *current = list->head;                     \
    while (index > 0) {                                                           \
        current = current->next;                                                  \
        index--;                                                                  \
    }                                                                             \
    return &current->data;                                                        \
}                                                                                 \
                                                                                  \
bool NAME##_gdsp_linked_list_pop_front(NAME##_gdsp_linked_list *list,             \
                                       TYPE *out_data) {                          \
    if (!list || list->size == 0) {                                               \
        return false;                                                             \
    }                                                                             \
                                                                                  \
    NAME##_gdsp_linked_list_node *node = list->head;                              \
    if (out_data) {                                                               \
        *out_data = node->data;                                                   \
    }                                                                             \
                                                                                  \
    list->head = node->next;                                                      \
    list->size--;                                                                 \
                                                                                  \
    if (list->size == 0) {                                                        \
        list->tail = NULL;                                                        \
    }                                                                             \
                                                                                  \
    free(node);                                                                   \
    return true;                                                                  \
}                                                                                 \
                                                                                  \
bool NAME##_gdsp_linked_list_pop_back(NAME##_gdsp_linked_list *list,              \
                                      TYPE *out_data) {                           \
    if (!list || list->size == 0) {                                               \
        return false;                                                             \
    }                                                                             \
                                                                                  \
    if (list->size == 1) {                                                        \
        return NAME##_gdsp_linked_list_pop_front(list, out_data);                 \
    }                                                                             \
                                                                                  \
    NAME##_gdsp_linked_list_node *previous = list->head;                          \
    while (previous->next != list->tail) {                                        \
        previous = previous->next;                                                \
    }                                                                             \
                                                                                  \
    NAME##_gdsp_linked_list_node *node = list->tail;                              \
    if (out_data) {                                                               \
        *out_data = node->data;                                                   \
    }                                                                             \
                                                                                  \
    previous->next = NULL;                                                        \
    list->tail = previous;                                                        \
    list->size--;                                                                 \
                                                                                  \
    free(node);                                                                   \
    return true;                                                                  \
}                                                                                 \
                                                                                  \
bool NAME##_gdsp_linked_list_pop_at(NAME##_gdsp_linked_list *list,                \
                                    size_t index,                                 \
                                    TYPE *out_data) {                             \
    if (!list || index >= list->size) {                                           \
        return false;                                                             \
    }                                                                             \
                                                                                  \
    if (index == 0) {                                                             \
        return NAME##_gdsp_linked_list_pop_front(list, out_data);                 \
    }                                                                             \
                                                                                  \
    if (index == list->size - 1) {                                                \
        return NAME##_gdsp_linked_list_pop_back(list, out_data);                  \
    }                                                                             \
                                                                                  \
    NAME##_gdsp_linked_list_node *previous = list->head;                          \
    while (index > 1) {                                                           \
        previous = previous->next;                                                \
        index--;                                                                  \
    }                                                                             \
                                                                                  \
    NAME##_gdsp_linked_list_node *node = previous->next;                          \
    if (out_data) {                                                               \
        *out_data = node->data;                                                   \
    }                                                                             \
                                                                                  \
    previous->next = node->next;                                                  \
    list->size--;                                                                 \
                                                                                  \
    free(node);                                                                   \
    return true;                                                                  \
}                                                                                 \
                                                                                  \
bool NAME##_gdsp_linked_list_remove_at(NAME##_gdsp_linked_list *list,             \
                                       size_t index) {                            \
    if (!list || index >= list->size) {                                           \
        return false;                                                             \
    }                                                                             \
                                                                                  \
    TYPE data;                                                                    \
    if (!NAME##_gdsp_linked_list_pop_at(list, index, &data)) {                    \
        return false;                                                             \
    }                                                                             \
                                                                                  \
    if (list->destroy_data) {                                                     \
        list->destroy_data(&data);                                                \
    }                                                                             \
                                                                                  \
    return true;                                                                  \
}

#endif