#include "rbe_array_list.h"

RBEArrayList* rbe_array_list_create(size_t size, size_t valueSize) {
    RBEArrayList* arrayList = RBE_MEM_ALLOCATE_SIZE(sizeof(RBEArrayList) + (valueSize * size));
    arrayList->size = size;
    arrayList->valueSize = valueSize;
    for (size_t i = 0; i < size; i++) {
        arrayList->values[i] = NULL;
    }
    return arrayList;
}
