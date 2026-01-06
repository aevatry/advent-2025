#include "array.h"
#include <assert.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>

int max_i(int a, int b) { return a * (a >= b) + b * (a < b); };
int min_i(int a, int b) { return a * (a <= b) + b * (a > b); };

IntArray *create_array_i(int max_n, int max_m) {
    IntArray *array = (IntArray *)malloc(sizeof(*array));
    array->max_n = max_n;
    array->max_m = max_m;

    array->data = (int **)malloc(max_n * sizeof(int *));
    for (int i = 0; i < max_n; i++) {
        array->data[i] = malloc(max_m * sizeof(int));
    }
    return array;
};

void free_array_i(IntArray *array) {
    for (int i = 0; i < array->max_n; i++) {
        free(array->data[i]);
    }
    free(array);
};

void fill_array_i(int val, IntArray *array) {
    for (int i = 0; i < array->max_n; i++) {
        for (int j = 0; j < array->max_m; j++) {
            array->data[i][j] = val;
        }
    }
};

void push_bounds_i(int n_inc, int m_inc, IntArray *array) {
    int n_end = min_i(n_inc, array->max_n);
    int m_end = min_i(m_inc, array->max_m);

    array->n = n_end;
    array->m = m_end;
};

void put_val_in_array_i(int val, int n_idx, int m_idx, IntArray *array) {
    assert(n_idx < array->max_n);
    assert(m_idx < array->max_m);

    if (array->n < n_idx) {
        array->n = n_idx;
    }
    if (array->m < m_idx) {
        array->m = m_idx;
    }

    array->data[n_idx][m_idx] = val;
};

/*
 * Unisgned Long
 */

unsigned long max_ulong(unsigned long a, unsigned long b) { return a * (a >= b) + b * (a < b); };
unsigned long min_ulong(unsigned long a, unsigned long b) { return a * (a <= b) + b * (a > b); };

ULongArray *create_array_ulong(int max_n, int max_m) {
    ULongArray *array = (ULongArray *)malloc(sizeof(*array));
    array->max_n = max_n;
    array->max_m = max_m;

    array->data = (unsigned long **)malloc(max_n * sizeof(unsigned long *));
    for (int i = 0; i < max_n; i++) {
        array->data[i] = malloc(max_m * sizeof(unsigned long));
    }
    return array;
};

void free_array_ulong(ULongArray *array) {
    for (int i = 0; i < array->max_n; i++) {
        free(array->data[i]);
    }
    free(array);
};

void fill_array_ulong(unsigned long val, ULongArray *array) {
    for (int i = 0; i < array->max_n; i++) {
        for (int j = 0; j < array->max_m; j++) {
            array->data[i][j] = val;
        }
    }
};

void push_bounds_ulong(int n_inc, int m_inc, ULongArray *array) {
    int n_end = min_i(n_inc, array->max_n);
    int m_end = min_i(m_inc, array->max_m);

    array->n = n_end;
    array->m = m_end;
};

void put_val_in_array_ulong(unsigned long val, int n_idx, int m_idx, ULongArray *array) {
    assert(n_idx < array->max_n);
    assert(m_idx < array->max_m);

    if (array->n < n_idx) {
        array->n = n_idx;
    }
    if (array->m < m_idx) {
        array->m = m_idx;
    }

    array->data[n_idx][m_idx] = val;
};
