#include "se_mem.h"

#include "../utils/logger.h"

void* se_mem_allocate(size_t size) {
    void* memory = calloc(1, size);
    if (memory == NULL) {
        se_logger_error("Out of memory or malloc failed!, size = %d", size);
    }
    return memory;
}

void* se_mem_allocate_c(int blocks, size_t size) {
    void* memory = calloc(blocks, size);
    if (memory == NULL) {
        se_logger_error("Out of memory or malloc failed!, size = %d", size);
    }
    return memory;
}

void se_mem_free(void* memory) {
    free(memory);
    memory = NULL;
}
