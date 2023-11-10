#pragma once

#include <stddef.h>
#include <stdbool.h>

#include "../memory/se_mem.h"

#define SE_ARRAY_LIST_FOR_EACH(list, element) for((element) = (list)->head; (element) != NULL; (element) = (element)->next)

// Should return whether the array list node is greater than the other
typedef bool (*SeArrayListIsGreaterThanFunc) (void*, void*);

// Seika implementation of a single linked list
typedef struct SEArrayListNode {
    struct SEArrayListNode* next;
    void* value;
} SEArrayListNode;

// Generic array list, use 'se_array_list_create' to allocate on the heap
typedef struct SEArrayList {
    size_t valueSize;
    size_t size;
    SEArrayListNode* head;
    SEArrayListNode* tail;
} SEArrayList;

// Will create a new array list with all it's element values set to the passed in 'valueSize'
SEArrayList* se_array_list_create(size_t valueSize);
// Will complete destroy the array list freeing up all its memory
void se_array_list_destroy(SEArrayList* list);

// Adds an item at the end of the list
void se_array_list_append(SEArrayList* list, void* value);
// Adds an item at the beginning of the list
void se_array_list_prepend(SEArrayList* list, void* value);
// Inserts an item at the specified index
void se_array_list_insert(SEArrayList* list, void* value, size_t index);

// Returns an item from the list from specified index
void* se_array_list_get(SEArrayList* list, size_t index);
// Same as 'se_array_list_get' but returns item from index 0
void* se_array_list_get_front(SEArrayList* list);
// Same as 'se_array_list_get' but returns item from index (size - 1)
void* se_array_list_get_back(SEArrayList* list);

// Removes and item and returns from the list from specified index, caller needs to manage memory
void* se_array_list_pop(SEArrayList* list, size_t index);
// Same as 'se_array_list_pop' but removes and item from index 0, caller needs to manage memory
void* se_array_list_pop_front(SEArrayList* list);
// Same as 'se_array_list_pop' but removes and item from index (size - 1), caller needs to manage memory
void* se_array_list_pop_back(SEArrayList* list);
// Removes the first item from the list who is equal to the passed in value
bool se_array_list_remove(SEArrayList* list, void* value);

// Returns true if the array list size == 0
bool se_array_list_is_empty(SEArrayList* list);
// Will sort the array list, using the passed in 'isGreaterThanFunc' as a comparator for sorting
void se_array_list_sort(SEArrayList* list, SeArrayListIsGreaterThanFunc isGreaterThanFunc);
// Will remove all items from the array list
void se_array_list_clear(SEArrayList* list);
