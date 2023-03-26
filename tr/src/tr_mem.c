#include "tr_mem.h"

#include <stdio.h>

void* tr_mem_allocate(size_t size) {
    void* memory = calloc(1, size);
    if (memory == NULL) {
        printf("Out of memory or malloc failed!, size = %d", size);
    }
    return memory;
}

void* tr_mem_allocate_c(int blocks, size_t size) {
    void* memory = calloc(blocks, size);
    if (memory == NULL) {
        printf("Out of memory or malloc failed!, size = %d", size);
    }
    return memory;
}

void tr_mem_free(void* memory) {
    free(memory);
    memory = NULL;
}
