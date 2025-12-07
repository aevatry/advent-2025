#include <assert.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>

#define LINE_LENGTH 256
#define ARRAY_SIZE_N 256
#define ARRAY_SIZE_M 256

typedef enum { false, true } bool;

int max(int a, int b) { return a * (a >= b) + b * (a < b); };
int min(int a, int b) { return a * (a <= b) + b * (a > b); };

typedef struct IntArray {
    int max_n;
    int n;
    int max_m;
    int m;
    int **data;
} IntArray;

IntArray *create_array(int max_n, int max_m) {
    IntArray *array = (IntArray *)malloc(sizeof(*array));
    array->max_n = max_n;
    array->max_m = max_m;

    array->data = (int **)malloc(max_n * sizeof(int *));
    for (int i = 0; i < max_n; i++) {
        array->data[i] = malloc(max_m * sizeof(int));
    }
    return array;
};

void free_array(IntArray *array) {
    for (int i = 0; i < array->max_n; i++) {
        free(array->data[i]);
    }
    free(array);
};

void fill_array(int val, IntArray *array) {
    for (int i = 0; i < array->max_n; i++) {
        for (int j = 0; j < array->max_m; j++) {
            array->data[i][j] = val;
        }
    }
};

void push_bounds(int n_inc, int m_inc, IntArray *array) {
    int n_end = min(n_inc, array->max_n);
    int m_end = min(m_inc, array->max_m);

    array->n = n_end;
    array->m = m_end;
};

void put_val_in_array(int val, int n_idx, int m_idx, IntArray *array) {
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

char get_from_array_as_char(int n_idx, int m_idx, IntArray *array) {
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

void print_2d_array(IntArray *array) {
    // Printing the 2D array
    for (int i = 0; i <= array->n; i++) {
        for (int j = 0; j <= array->m; j++) {
            printf("%i ", array->data[i][j]);
        }
        printf("\n"); // Move to the next row
    }
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
    IntArray *diagram = create_array(ARRAY_SIZE_N, ARRAY_SIZE_M);
    fill_array((int)'.', diagram);

    // setup of problem variables
    int splitter_counter = 0;
    bool has_reached_eol;
    int line_counter = 0;
    int in_line_idx;

    while (fgets(line, LINE_LENGTH, input_file) != NULL) {

        // reset line interpretation
        has_reached_eol = false;
        in_line_idx = 0;

        // !! assumption -> input is well formatted
        while (has_reached_eol == false) {

            push_bounds(line_counter + 1, in_line_idx + 1, diagram);

            // base case of first line
            if (line[in_line_idx] == 'S') {
                put_val_in_array((int)'S', line_counter, in_line_idx, diagram);
                put_val_in_array((int)'|', line_counter + 1, in_line_idx, diagram);
            }

            // case of |
            if (get_from_array_as_char(line_counter - 1, in_line_idx, diagram) == '|') {
                put_val_in_array((int)'|', line_counter, in_line_idx, diagram);
            }

            // case of ^
            if (line[in_line_idx] == '^') {
                put_val_in_array((int)'^', line_counter, in_line_idx, diagram);

                // if there is a beam above
                if (get_from_array_as_char(line_counter - 1, in_line_idx, diagram) == '|') {
                    splitter_counter += 1;
                    put_val_in_array((int)'|', line_counter, in_line_idx - 1, diagram);
                    put_val_in_array((int)'|', line_counter, in_line_idx + 1, diagram);
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

    for (int i = 0; i <= diagram->n; i++) {

        for (int j = 0; j <= diagram->m; j++) {
            char curr_char = (char)diagram->data[i][j];
            printf("%c", curr_char);
        }
        printf("\n");
    }

    free_array(diagram);

    printf("Total number of splits: %i\n", splitter_counter);

    fclose(input_file);
    return EXIT_SUCCESS;
}
