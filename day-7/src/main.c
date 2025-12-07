#include "array.h"
#include <assert.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>

#define LINE_LENGTH 256
#define ARRAY_SIZE_N 256
#define ARRAY_SIZE_M 256

typedef enum { false, true } bool;

char get_from_int_array_as_char(int n_idx, int m_idx, IntArray *array) {
    char res;

    // bounds checking
    if (0 <= n_idx && n_idx < array->max_n && 0 <= m_idx && m_idx < array->max_m) {
        res = (char)array->data[n_idx][m_idx];
    } else {
        printf("Warning: position (%i, %i) does not exist, returning 0\n", n_idx, m_idx);
        res = 0;
    }

    return res;
};

void print_2d_int_array(IntArray *array) {
    // Printing the 2D array
    for (int i = 0; i <= array->n; i++) {
        for (int j = 0; j <= array->m; j++) {
            printf("%i ", array->data[i][j]);
        }
        printf("\n"); // Move to the next row
    }
};

// could add caching but a bit more complicated
unsigned long number_of_paths(IntArray *array, int idx, int depth, ULongArray *cache) {

    unsigned long res = 0;

    // base case
    if (depth == 0) {
        return 1;
    }

    int n = array->n - depth;

    if (cache->data[n][idx] > 0) {
        return cache->data[n][idx];
    }

    if (get_from_int_array_as_char(n, idx, array) == '|') {
        res = number_of_paths(array, idx, depth - 1, cache);
    }
    if (get_from_int_array_as_char(n, idx, array) == '^') {
        res = number_of_paths(array, idx - 1, depth - 1, cache) +
              number_of_paths(array, idx + 1, depth - 1, cache);
    }

    cache->data[n][idx] = res;

    return res;
};

int main() {

    FILE *input_file;
    input_file = fopen("input.txt", "r");
    if (input_file == NULL) {
        printf("The file is not opened.");
        fclose(input_file);
        return EXIT_FAILURE;
    }

    /*Each line ends with 2 characters -> newline/line feed (ASCII 10) and NULL (ASCII 0)  */
    char line[LINE_LENGTH];

    // start state of the problem
    IntArray *diagram = create_array_i(ARRAY_SIZE_N, ARRAY_SIZE_M);
    fill_array_i((int)'.', diagram);

    // setup of problem variables
    int idx_first = 0;
    bool has_reached_eol;
    int line_counter = 0;
    int in_line_idx;

    while (fgets(line, LINE_LENGTH, input_file) != NULL) {

        // reset line interpretation
        has_reached_eol = false;
        in_line_idx = 0;

        // !! assumption -> input is well formatted
        while (has_reached_eol == false) {

            push_bounds_i(line_counter + 1, in_line_idx + 1, diagram);

            // base case of first line
            if (line[in_line_idx] == 'S') {
                idx_first = in_line_idx;
                put_val_in_array_i((int)'|', line_counter, in_line_idx, diagram);
            }

            // case of |
            if (line_counter > 0 &&
                get_from_int_array_as_char(line_counter - 1, in_line_idx, diagram) == '|') {
                put_val_in_array_i((int)'|', line_counter, in_line_idx, diagram);
            }

            // case of ^
            if (line[in_line_idx] == '^') {
                put_val_in_array_i((int)'^', line_counter, in_line_idx, diagram);

                // if there is a beam above
                if (get_from_int_array_as_char(line_counter - 1, in_line_idx, diagram) == '|') {
                    put_val_in_array_i((int)'|', line_counter, in_line_idx - 1, diagram);
                    put_val_in_array_i((int)'|', line_counter, in_line_idx + 1, diagram);
                }
            }

            in_line_idx++;
            // line end condition
            if ((int)line[in_line_idx] == 10) {
                has_reached_eol = true;
            }
        }
        line_counter++;
    }

    push_bounds_i(line_counter - 1, in_line_idx - 1, diagram);

    for (int i = 0; i <= diagram->n; i++) {

        for (int j = 0; j <= diagram->m; j++) {
            char curr_char = (char)diagram->data[i][j];
            printf("%c", curr_char);
        }
        printf("\n");
    }

    ULongArray *cache = create_array_ulong(diagram->n, diagram->m);
    fill_array_ulong(0, cache);

    unsigned long num_paths = number_of_paths(diagram, idx_first, diagram->n, cache);

    free_array_i(diagram);
    free_array_ulong(cache);

    printf("Num paths: %lu\n", num_paths);

    fclose(input_file);
    return EXIT_SUCCESS;
}
