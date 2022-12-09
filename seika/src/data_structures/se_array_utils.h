#pragma once

#include <stddef.h>
#include <stdint.h>

#define SE_ARRAY_UTILS_REMOVE_ARRAY_ITEM(ARRAY, ARRAY_SIZE, ELEMENT, EMPTY_VALUE) \
{                                                                                       \
const size_t temp_arac_size = (ARRAY_SIZE);                                                                                        \
for (size_t temp_arac_loop_index = 0; temp_arac_loop_index < temp_arac_size; temp_arac_loop_index++) {       \
if (ARRAY[temp_arac_loop_index] == ELEMENT) {                      \
ARRAY[temp_arac_loop_index] = ARRAY[temp_arac_loop_index + 1];                       \
ARRAY[temp_arac_loop_index + 1] = EMPTY_VALUE;                         \
(ARRAY_SIZE)--;                                   \
}                                                        \
if (ARRAY[temp_arac_loop_index] == EMPTY_VALUE && temp_arac_loop_index + 1 < temp_arac_size) {                      \
ARRAY[temp_arac_loop_index] = ARRAY[temp_arac_loop_index + 1];                       \
ARRAY[temp_arac_loop_index + 1] = EMPTY_VALUE;                         \
}                                                        \
}                                                        \
}

// Function to perform Selection Sort
void se_array_utils_selection_sort_int(int arr[], int arraySize);

void se_array_utils_remove_item_uint32(uint32_t array[], size_t* size, uint32_t item, uint32_t emptyValue);
