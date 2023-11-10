#pragma once

#include <stdlib.h>

#ifdef __cplusplus
extern "C" {
#endif

#define SE_MEM_ALLOCATE(DataType)             \
(DataType*) se_mem_allocate(sizeof(DataType))

#define SE_MEM_ALLOCATE_SIZE(Size)             \
se_mem_allocate(Size)

#define SE_MEM_ALLOCATE_SIZE_ZERO(Blocks, Size)             \
se_mem_allocate_c(Blocks, Size)

#define SE_MEM_FREE(Memory)             \
se_mem_free(Memory)

void* se_mem_allocate(size_t size);
void* se_mem_allocate_c(int blocks, size_t size);
void se_mem_free(void* memory);

#ifdef __cplusplus
}
#endif
