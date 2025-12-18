#ifndef MY_ARRAY_H
#define MY_ARRAY_H

#include <stdlib.h>
#include <math.h>

int max_i(int a, int b);
int min_i(int a, int b);

typedef struct IntArray {
    int max_n;
    int n;
    int max_m;
    int m;
    int **data;
} IntArray;

IntArray *create_array_i(int max_n, int max_m);
void free_array_i(IntArray *array);
void fill_array_i(int val, IntArray *array);
void push_bounds_i(int n_inc, int m_inc, IntArray *array);
void put_val_in_array_i(int val, int n_idx, int m_idx, IntArray *array);

// Unisgned long
unsigned long max_ulong(unsigned long a, unsigned long b);
unsigned long min_ulong(unsigned long a, unsigned long b);

typedef struct UnisgnedLongArray {
    int max_n;
    int n;
    int max_m;
    int m;
    unsigned long **data;
} ULongArray;

ULongArray *create_array_ulong(int max_n, int max_m);
void free_array_ulong(ULongArray *array);
void fill_array_ulong(unsigned long val, ULongArray *array);
void push_bounds_ulong(int n_inc, int m_inc, ULongArray *array);
void put_val_in_array_ulong(unsigned long val, int n_idx, int m_idx, ULongArray *array);



#endif
