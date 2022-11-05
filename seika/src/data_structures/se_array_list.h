#pragma once

#include <stddef.h>
#include <stdbool.h>

#include "../memory/se_mem.h"

typedef struct SEArrayListNode {
    struct SEArrayListNode* next;
    void* value;
} SEArrayListNode;

typedef struct SEArrayList {
    size_t valueSize;
    size_t capacity;
    size_t size;
    SEArrayListNode** nodes;
} SEArrayList;

SEArrayList* se_array_list_create(size_t size, size_t valueSize);
void se_array_list_add_value(void* value);
void se_array_list_get_value(size_t index);
bool se_array_list_is_empty(size_t size, size_t valueSize);
