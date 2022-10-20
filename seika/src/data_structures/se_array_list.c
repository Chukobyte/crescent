#include "se_array_list.h"

SEArrayList* se_array_list_create(size_t size, size_t valueSize) {
    SEArrayList* arrayList = SE_MEM_ALLOCATE_SIZE(sizeof(SEArrayList) + (valueSize * size));
    arrayList->size = size;
    arrayList->valueSize = valueSize;
    for (size_t i = 0; i < size; i++) {
        arrayList->values[i] = NULL;
    }
    return arrayList;
}
