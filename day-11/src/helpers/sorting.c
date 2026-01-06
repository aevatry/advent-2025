#include "sorting.h"

void swap_l(long *a, long *b) {
    long temp = *a;
    *a = *b;
    *b = temp;
}
void swap_i(int *a, int *b) {
    int temp = *a;
    *a = *b;
    *b = temp;
}

// Partition function for long arrays
int partition_l(long *arr, int *indices, int low, int high) {
    long pivot = arr[high];
    int i = low;
    for (int j = low; j < high; j++) {
        if (arr[j] <= pivot) {
            swap_l(&arr[i], &arr[j]);
            swap_i(&indices[i], &indices[j]);
            i++;
        }
    }
    swap_l(&arr[i], &arr[high]);
    swap_i(&indices[i], &indices[high]);
    return i;
};

// Quick Sort function for an array of long. Keeps track of sorted indices
void quick_sort_l(long *arr, int *indices, int low, int high) {
    if (low < high) {
        int pi = partition_l(arr, indices, low, high);
        quick_sort_l(arr, indices, low, pi - 1);
        quick_sort_l(arr, indices, pi + 1, high);
    }
}

// Partition function for int arrays
int partition_i(int *arr, int *indices, int low, int high) {
    long pivot = arr[high];
    int i = low;
    for (int j = low; j < high; j++) {
        if (arr[j] <= pivot) {
            swap_i(&arr[i], &arr[j]);
            swap_i(&indices[i], &indices[j]);
            i++;
        }
    }
    swap_i(&arr[i], &arr[high]);
    swap_i(&indices[i], &indices[high]);
    return i;
};

// Quick Sort function for an array of int. Keeps track of sorted indices
void quick_sort_i(int *arr, int *indices, int low, int high) {
    if (low < high) {
        int pi = partition_i(arr, indices, low, high);
        quick_sort_i(arr, indices, low, pi - 1);
        quick_sort_i(arr, indices, pi + 1, high);
    }
}
