#pragma once

#include <stddef.h>

#include "../memory/rbe_mem.h"

typedef struct RBEArrayList {
    size_t size;
    size_t valueSize;
    void* values[];
} RBEArrayList;

RBEArrayList* rbe_array_list_create(size_t size, size_t valueSize);
