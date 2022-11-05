#pragma once

inline void se_array_utils_swap(int* xp, int* yp) {
    int temp = *xp;
    *xp = *yp;
    *yp = temp;
}

// Function to perform Selection Sort
inline void se_array_utils_selection_sort_int(int arr[], int arraySize) {
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
