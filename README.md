# GDSP — Generic Data Structures in Pure C

> A lightweight, type-safe generic data structures library for C, built with macros.

---

## Table of Contents

- [Overview](#overview)
- [How It Works](#how-it-works)
- [Installation](#installation)
- [Data Structures](#data-structures)
  - [Vector](#vector)
  - [Linked List](#linked-list)
  - [Double Linked List](#double-linked-list)
  - [Queue](#queue)
  - [Stack](#stack)
- [API Reference](#api-reference)
- [Examples](#examples)

---

## Overview

**GDSP** (Generic Data Structures in Pure C) is a header-only C library providing generic, type-safe data structures through the use of C preprocessor macros. No need for `void *` casting or manual type management — just declare your structure for any type and use it.

---

## How It Works

GDSP uses a two-macro pattern:

| Macro | Purpose | Where to use |
|---|---|---|
| `DECLARE_GDSP_<STRUCTURE>(TYPE, NAME)` | Declares the types and function prototypes | `.h` files |
| `BUILDING_GDSP_<STRUCTURE>(TYPE, NAME)` | Defines the actual implementations | **one** `.c` file only |

- **`TYPE`** — the data type the structure will hold (e.g. `int`, `float`, `MyStruct`)
- **`NAME`** — a unique prefix used to namespace all generated types and functions

> ⚠️ `BUILDING_GDSP_*` must be used **exactly once** per `(TYPE, NAME)` pair across your project to avoid multiple definition errors.

---

## Installation

GDSP is a **header-only** library. Simply copy the relevant headers into your project:

```
your_project/
├── gdsp_vector.h
├── gdsp_linked_list.h
├── gdsp_double_linked_list.h
├── gdsp_queue.h
└── gdsp_stack.h
```

No compilation or linking step is required for the library itself.

---

## Data Structures

---

### Vector

A dynamic array that grows automatically when its capacity is exceeded. Elements are stored contiguously in memory, enabling fast random access by index.

**Header:** `gdsp_vector.h`

#### Setup

**`my_vector.h`**
```c
#include "gdsp_vector.h"

DECLARE_GDSP_VECTOR(int, int)
```

**`my_vector.c`**
```c
#include "my_vector.h"

BUILDING_GDSP_VECTOR(int, int)
```

#### Usage

```c
#include "my_vector.h"

int main(void) {
    int_gdsp_vector *vec = int_gdsp_vector_create(NULL);

    int_gdsp_vector_push_back(vec, 10);
    int_gdsp_vector_push_back(vec, 20);
    int_gdsp_vector_push_front(vec, 5);

    int *val = int_gdsp_vector_get_at(vec, 1);
    printf("%d\n", *val); // 10

    int out;
    int_gdsp_vector_pop_back(vec, &out);
    printf("Popped: %d\n", out); // 20

    int_gdsp_vector_destroy(vec);
    return 0;
}
```

---

### Linked List

A singly linked list. Elements are stored as individual nodes, each pointing to the next. Efficient for insertions and deletions at the front.

**Header:** `gdsp_linked_list.h`

#### Setup

**`my_list.h`**
```c
#include "gdsp_linked_list.h"

DECLARE_GDSP_LINKED_LIST(float, float)
```

**`my_list.c`**
```c
#include "my_list.h"

BUILDING_GDSP_LINKED_LIST(float, float)
```

#### Usage

```c
#include "my_list.h"

int main(void) {
    float_gdsp_linked_list *list = float_gdsp_linked_list_create(NULL);

    float_gdsp_linked_list_push_back(list, 1.1f);
    float_gdsp_linked_list_push_back(list, 2.2f);
    float_gdsp_linked_list_push_front(list, 0.0f);

    float *front = float_gdsp_linked_list_front(list);
    printf("%f\n", *front); // 0.0

    float_gdsp_linked_list_destroy(list);
    return 0;
}
```

---

### Double Linked List

A doubly linked list. Each node holds a pointer to both the next and the previous node, allowing efficient traversal and deletion in both directions.

**Header:** `gdsp_double_linked_list.h`

#### Setup

**`my_dlist.h`**
```c
#include "gdsp_double_linked_list.h"

DECLARE_GDSP_DOUBLE_LINKED_LIST(int, int)
```

**`my_dlist.c`**
```c
#include "my_dlist.h"

BUILDING_GDSP_DOUBLE_LINKED_LIST(int, int)
```

#### Usage

```c
#include "my_dlist.h"

int main(void) {
    int_gdsp_d_linked_list *list = int_gdsp_d_linked_list_create(NULL);

    int_gdsp_d_linked_list_push_back(list, 100);
    int_gdsp_d_linked_list_push_back(list, 200);
    int_gdsp_d_linked_list_insert_at(list, 150, 1);

    int *val = int_gdsp_d_linked_list_get_at(list, 1);
    printf("%d\n", *val); // 150

    int out;
    int_gdsp_d_linked_list_pop_front(list, &out);
    printf("Popped front: %d\n", out); // 100

    int_gdsp_d_linked_list_destroy(list);
    return 0;
}
```

---

### Queue

A FIFO (First In, First Out) structure. Elements are enqueued at the back and dequeued from the front.

**Header:** `gdsp_queue.h`

#### Setup

**`my_queue.h`**
```c
#include "gdsp_queue.h"

DECLARE_GDSP_QUEUE(int, int)
```

**`my_queue.c`**
```c
#include "my_queue.h"

BUILDING_GDSP_QUEUE(int, int)
```

#### Usage

```c
#include "my_queue.h"

int main(void) {
    int_gdsp_queue *queue = int_gdsp_queue_create(NULL);

    int_gdsp_queue_push(queue, 1);
    int_gdsp_queue_push(queue, 2);
    int_gdsp_queue_push(queue, 3);

    int out;
    int_gdsp_queue_pop(queue, &out);
    printf("Dequeued: %d\n", out); // 1

    printf("Size: %zu\n", int_gdsp_queue_size(queue)); // 2

    int_gdsp_queue_destroy(queue);
    return 0;
}
```

---

### Stack

A LIFO (Last In, First Out) structure. Elements are pushed and popped from the top.

**Header:** `gdsp_stack.h`

#### Setup

**`my_stack.h`**
```c
#include "gdsp_stack.h"

DECLARE_GDSP_STACK(int, int)
```

**`my_stack.c`**
```c
#include "my_stack.h"

BUILDING_GDSP_STACK(int, int)
```

#### Usage

```c
#include "my_stack.h"

int main(void) {
    int_gdsp_stack *stack = int_gdsp_stack_create(NULL);

    int_gdsp_stack_push(stack, 10);
    int_gdsp_stack_push(stack, 20);
    int_gdsp_stack_push(stack, 30);

    int *top = int_gdsp_stack_peek(stack);
    printf("Top: %d\n", *top); // 30

    int out;
    int_gdsp_stack_pop(stack, &out);
    printf("Popped: %d\n", out); // 30

    int_gdsp_stack_destroy(stack);
    return 0;
}
```

---

## API Reference

### Common Functions

All structures share the following function pattern (replace `<struct>` with `vector`, `linked_list`, `d_linked_list`, `queue` or `stack`, and `NAME` with your chosen prefix):

| Function | Description | Returns |
|---|---|---|
| `NAME_gdsp_<struct>_create(destroy_data)` | Allocates and initializes a new structure. Pass `NULL` if no custom destructor is needed. | Pointer to structure or `NULL` on failure |
| `NAME_gdsp_<struct>_destroy(s)` | Frees all elements and the structure itself. | `void` |
| `NAME_gdsp_<struct>_clear(s)` | Removes all elements but keeps the structure alive. | `void` |
| `NAME_gdsp_<struct>_empty(s)` | Returns `true` if the structure contains no elements. | `bool` |
| `NAME_gdsp_<struct>_size(s)` | Returns the number of elements currently stored. | `size_t` |

---

### Vector — Additional Functions

| Function | Description | Returns |
|---|---|---|
| `NAME_gdsp_vector_reserve(v, n)` | Pre-allocates capacity for at least `n` elements. | `bool` |
| `NAME_gdsp_vector_capacity(v)` | Returns the current allocated capacity. | `size_t` |
| `NAME_gdsp_vector_push_front(v, data)` | Inserts an element at the beginning. | `bool` |
| `NAME_gdsp_vector_push_back(v, data)` | Appends an element at the end. | `bool` |
| `NAME_gdsp_vector_insert_at(v, data, i)` | Inserts an element at index `i`. | `bool` |
| `NAME_gdsp_vector_front(v)` | Returns a pointer to the first element. | `TYPE *` |
| `NAME_gdsp_vector_back(v)` | Returns a pointer to the last element. | `TYPE *` |
| `NAME_gdsp_vector_get_at(v, i)` | Returns a pointer to the element at index `i`. | `TYPE *` |
| `NAME_gdsp_vector_pop_front(v, out)` | Removes the first element and copies it into `out`. | `bool` |
| `NAME_gdsp_vector_pop_back(v, out)` | Removes the last element and copies it into `out`. | `bool` |
| `NAME_gdsp_vector_pop_at(v, i, out)` | Removes the element at index `i` and copies it into `out`. | `bool` |

---

### Linked List & Double Linked List — Additional Functions

| Function | Description | Returns |
|---|---|---|
| `NAME_gdsp_<list>_push_front(l, data)` | Inserts an element at the front. | `bool` |
| `NAME_gdsp_<list>_push_back(l, data)` | Appends an element at the back. | `bool` |
| `NAME_gdsp_<list>_insert_at(l, data, i)` | Inserts an element at index `i`. | `bool` |
| `NAME_gdsp_<list>_front(l)` | Returns a pointer to the first element. | `TYPE *` |
| `NAME_gdsp_<list>_back(l)` | Returns a pointer to the last element. | `TYPE *` |
| `NAME_gdsp_<list>_get_at(l, i)` | Returns a pointer to the element at index `i`. | `TYPE *` |
| `NAME_gdsp_<list>_pop_front(l, out)` | Removes and retrieves the first element. | `bool` |
| `NAME_gdsp_<list>_pop_back(l, out)` | Removes and retrieves the last element. | `bool` |
| `NAME_gdsp_<list>_pop_at(l, i, out)` | Removes and retrieves the element at index `i`. | `bool` |
| `NAME_gdsp_<list>_remove_at(l, i)` | Removes the element at index `i` without retrieving it. | `bool` |

---

### Queue — Additional Functions

| Function | Description | Returns |
|---|---|---|
| `NAME_gdsp_queue_push(q, data)` | Enqueues an element at the back. | `bool` |
| `NAME_gdsp_queue_pop(q, out)` | Dequeues the front element into `out`. | `bool` |
| `NAME_gdsp_queue_peek(q)` | Returns a pointer to the front element without removing it. | `TYPE *` |

---

### Stack — Additional Functions

| Function | Description | Returns |
|---|---|---|
| `NAME_gdsp_stack_push(s, data)` | Pushes an element onto the top of the stack. | `bool` |
| `NAME_gdsp_stack_pop(s, out)` | Pops the top element into `out`. | `bool` |
| `NAME_gdsp_stack_peek(s)` | Returns a pointer to the top element without removing it. | `TYPE *` |

---

## Examples

### Using a custom struct with a destructor

```c
// point.h
#include "gdsp_vector.h"
#include <stdlib.h>

typedef struct {
    char *label;
    float x;
    float y;
} Point;

void point_destroy(Point *p) {
    free(p->label);
}

DECLARE_GDSP_VECTOR(Point, point)
```

```c
// point.c
#include "point.h"

BUILDING_GDSP_VECTOR(Point, point)
```

```c
// main.c
#include "point.h"
#include <string.h>
#include <stdio.h>

int main(void) {
    point_gdsp_vector *vec = point_gdsp_vector_create(point_destroy);

    Point p1 = { strdup("Origin"), 0.0f, 0.0f };
    Point p2 = { strdup("A"),      1.0f, 2.0f };

    point_gdsp_vector_push_back(vec, p1);
    point_gdsp_vector_push_back(vec, p2);

    Point *p = point_gdsp_vector_get_at(vec, 0);
    printf("Point: %s (%.1f, %.1f)\n", p->label, p->x, p->y);

    // destroy_data is called automatically on each element
    point_gdsp_vector_destroy(vec);
    return 0;
}
```

---

> Made with ❤️ in pure C.
