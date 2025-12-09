#ifndef MY_SORTING_H
#define MY_SORTING_H

// swap utils
void swap_l(long *a, long *b);
void swap_i(int *a, int *b);

// long sorting
int partition_l(long *arr, int *indices, int low, int high);
void quick_sort_l(long *arr, int *indices, int low, int high);

// int sorting
int partition_i(int *arr, int *indices, int low, int high);
void quick_sort_i(int *arr, int *indices, int low, int high);

#endif
