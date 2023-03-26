#pragma once

#include <stdlib.h>

#ifdef __cplusplus
extern "C" {
#endif

#define TR_MEM_ALLOCATE(DataType)             \
(DataType*) tr_mem_allocate(sizeof(DataType))

#define TR_MEM_ALLOCATE_SIZE(Size)             \
tr_mem_allocate(Size)

#define TR_MEM_ALLOCATE_SIZE_ZERO(Blocks, Size)             \
tr_mem_allocate_c(Blocks, Size)

#define TR_MEM_FREE(Memory)             \
tr_mem_free(Memory)

void* tr_mem_allocate(size_t size);
void* tr_mem_allocate_c(int blocks, size_t size);
void tr_mem_free(void* memory);

#ifdef __cplusplus
}
#endif
