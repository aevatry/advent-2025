#include <assert.h>
#include <limits.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#define LINE_LENGTH 8000
#define ARRAY_SIZE_N 1600
#define ARRAY_SIZE_M 16
#define MAX_NUM_LENGTH 12

typedef enum { false, true } bool;

int max_i(int a, int b) { return a * (a >= b) + b * (a < b); };
int min_i(int a, int b) { return a * (a <= b) + b * (a > b); };

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

void print_2d_int_array(IntArray *array) {
    // Printing the 2D array
    for (int i = 0; i <= array->n; i++) {
        for (int j = 0; j <= array->m; j++) {
            printf("%i ", array->data[i][j]);
        }
        printf("\n"); // Move to the next row
    }
};

int construct_int_from_chars(int length_num, char *chars) {

    int result = 0;
    for (int i = 1; i <= length_num; i++) {
        result += (chars[i - 1] - '0') * (int)pow(10, length_num - i);
    }
    return result;
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
    IntArray *file_copy_array = create_array(LINE_LENGTH, ARRAY_SIZE_M);
    char operands[ARRAY_SIZE_N];

    // setup of problem variables
    bool has_reached_eol;
    int line_counter = 0;
    int num_counter;
    int line_idx;

    while (fgets(line, LINE_LENGTH, input_file) != NULL) {

        // reset line interpretation
        has_reached_eol = false;
        num_counter = 0;
        line_idx = 0;

        // !! assumption -> input is well formatted
        while (has_reached_eol == false) {

            // Operands or main bulk of file
            if (line[line_idx] == '*' || line[line_idx] == '+') {
                operands[num_counter] = line[line_idx];
                num_counter++;
            } else {
                put_val_in_array((int)line[line_idx], line_idx, line_counter, file_copy_array);
            }

            line_idx++;
            // line end condition
            if ((int)line[line_idx] == 10) {
                has_reached_eol = true;
            }
        }
        line_counter++;
    }

    // Handling of the well formatted input
    unsigned long tot_sum = 0;

    unsigned long running_sum = 0;
    unsigned long running_product = 1;
    int operands_idx = 0;

    for (int i = 0; i <= file_copy_array->n; i++) {

        int number_length = 0;
        char char_buffer[MAX_NUM_LENGTH];
        bool line_empty = true;

        for (int j = 0; j <= file_copy_array->m; j++) {
            char curr_char = (char)file_copy_array->data[i][j];
            printf("%c", curr_char);
            if ('0' <= curr_char && curr_char <= '9') {
                char_buffer[number_length] = curr_char;
                number_length++;
                line_empty = false;
            }
        }

        if (line_empty == true) {
            char operand = operands[operands_idx];

            if (operand == '+') {
                tot_sum += running_sum;
                printf("Chose : %lu", running_sum);
            } else {
                tot_sum += running_product;
                printf(" Chose : %lu", running_product);
            }

            running_product = 1;
            running_sum = 0;
            operands_idx += 1;

        } else {
            int num = construct_int_from_chars(number_length, char_buffer);
            running_sum += (unsigned long)num;
            running_product *= (unsigned long)num;
        }

        printf("\n");
    }

    // need to add the last operand as it is not an empty line after
    char operand = operands[operands_idx];
    printf("At end of text, operand %c and totals are +: %lu and *: %lu", operands[operands_idx],
           running_sum, running_product);
    if (operand == '+') {
        tot_sum += running_sum;
        printf(" Chose : %lu \n", running_sum);
    } else {
        tot_sum += running_product;
        printf(" Chose : %lu \n", running_product);
    }

    free_array(file_copy_array);

    printf("Total sum: %lu\n", tot_sum);

    fclose(input_file);
    return EXIT_SUCCESS;
}
