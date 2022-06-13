#pragma once

#include <stddef.h>

#include "../memory/rbe_mem.h"

typedef struct RBEArrayList {
    size_t size;
    size_t valueSize;
    void* values[];
} RBEArrayList;

RBEArrayList rbe_array_list_create_on_stack(size_t size, size_t valueSize);
RBEArrayList* rbe_array_list_create_on_heap(size_t size, size_t valueSize);
