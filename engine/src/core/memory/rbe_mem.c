#include "rbe_mem.h"

#include <stdlib.h>

#include "../utils/logger.h"

void* rbe_mem_allocate(size_t size) {
    void* memory = calloc(1, size);
    if (memory == NULL) {
        rbe_logger_error("Out of memory or malloc failed!, size = %d", size);
    }
    return memory;
}

void rbe_mem_free(void* memory) {
    free(memory);
    memory = NULL;
}
