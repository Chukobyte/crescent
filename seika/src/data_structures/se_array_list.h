#pragma once

#include <stddef.h>

#include "../memory/se_mem.h"

typedef struct SEArrayList {
    size_t size;
    size_t valueSize;
    void* values[];
} SEArrayList;

SEArrayList* se_array_list_create(size_t size, size_t valueSize);
