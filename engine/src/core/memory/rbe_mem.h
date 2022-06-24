#pragma once

#include <stdlib.h>

#ifdef __cplusplus
extern "C" {
#endif

#define RBE_MEM_ALLOCATE(DataType)             \
(DataType*) rbe_mem_allocate(sizeof(DataType))

#define RBE_MEM_ALLOCATE_SIZE(Size)             \
rbe_mem_allocate(Size)

#define RBE_MEM_FREE(Memory)             \
rbe_mem_free(Memory)

void* rbe_mem_allocate(size_t size);
void rbe_mem_free(void* memory);

#ifdef __cplusplus
}
#endif
