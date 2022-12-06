#include "se_array_utils.h"

void se_array_utils_swap(int* xp, int* yp) {
    int temp = *xp;
    *xp = *yp;
    *yp = temp;
}

// Function to perform Selection Sort
void se_array_utils_selection_sort_int(int arr[], int arraySize) {
    int min_idx;

    // One by one move boundary of unsorted subarray
    for (int i = 0; i < arraySize - 1; i++) {

        // Find the minimum element in unsorted array
        min_idx = i;
        for (int j = i + 1; j < arraySize; j++) {
            if (arr[j] < arr[min_idx]) {
                min_idx = j;
            }
        }

        // Swap the found minimum element
        // with the first element
        se_array_utils_swap(&arr[min_idx], &arr[i]);
    }
}

void se_array_utils_remove_item_uint32(uint32_t array[], size_t* size, uint32_t item, uint32_t emptyValue) {
    const size_t countRef = *size;
    for (size_t i = 0; i < countRef; i++) {
        if (array[i] == item) {
            array[i] = array[i + 1];
            array[i + 1] = emptyValue;
            *size = *size - 1;
        }
        if (array[i] == emptyValue && i + 1 < countRef) {
            array[i] = array[i + 1];
            array[i + 1] = emptyValue;
        }
    }
}
