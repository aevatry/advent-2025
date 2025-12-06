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

void print_2d_array(IntArray *array) {
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
    unsigned long tot_sum = 0;
    IntArray *digits_array = create_array(ARRAY_SIZE_N, ARRAY_SIZE_M);
    char operands[ARRAY_SIZE_N];

    // setup of problem variables
    bool has_reached_eol;
    int line_counter = 0;
    int num_counter;
    int line_idx;

    // number reading within line
    bool is_within_number;
    int number_length;
    char char_buffer[MAX_NUM_LENGTH];

    while (fgets(line, LINE_LENGTH, input_file) != NULL) {

        // reset line interpretation
        has_reached_eol = false;
        num_counter = 0;
        line_idx = 0;

        // line starts with a number or an operand
        if ('0' <= line[line_idx] && line[line_idx] <= '9') {
            is_within_number = true;
        } else {
            is_within_number = false;
        }

        number_length = 0;

        // !! assumption -> input is well formatted
        while (has_reached_eol == false) {

            // read number
            if ('0' <= line[line_idx] && line[line_idx] <= '9') {
                assert(number_length < MAX_NUM_LENGTH);
                char_buffer[number_length] = line[line_idx];
                number_length += 1;
            }

            // process number when at end of it
            if (is_within_number == true &&
                (line[line_idx + 1] == ' ' || line[line_idx + 1] == 10)) {
                int num = construct_int_from_chars(number_length, char_buffer);
                put_val_in_array(num, num_counter, line_counter, digits_array);
                is_within_number = false;
                number_length = 0;
                num_counter++;
            }

            // detect start of a new number
            if (line[line_idx] == ' ' && '0' <= line[line_idx + 1] && line[line_idx + 1] <= '9') {
                is_within_number = true;
            }

            // Detect operands
            if (line[line_idx] == '*' || line[line_idx] == '+') {
                operands[num_counter] = line[line_idx];
                num_counter++;
            }

            line_idx++;
            // line end condition
            if ((int)line[line_idx] == 10) {
                has_reached_eol = true;
            }
        }
        line_counter++;
    }

    // print_2d_array(digits_array);
    // printf("%s \n", operands);

    for (int i = 0; i <= digits_array->n; i++) {

        unsigned long local_op_res = 0;
        if (operands[i] == '*') {
            local_op_res = 1;
        }
        for (int j = 0; j <= digits_array->m; j++) {
            if (operands[i] == '+') {
                local_op_res += (unsigned long)digits_array->data[i][j];
            } else {
                local_op_res *= (unsigned long)digits_array->data[i][j];
            }
        }
        // printf("For line %i, got operand %c and res: %lu\n", i, operands[i], local_op_res);
        tot_sum += local_op_res;
    }

    free_array(digits_array);

    printf("Total sum: %lu\n", tot_sum);

    fclose(input_file);
    return EXIT_SUCCESS;
}
