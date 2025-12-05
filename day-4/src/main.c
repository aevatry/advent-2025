#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#define LINE_LENGTH 256

typedef enum { false, true } bool;

typedef struct IntArray {
    int n;
    int max_n;
    int m;
    int max_m;
    int **data;
} IntArray;

IntArray *create_array(int max_n, int max_m) {
    IntArray *array = (IntArray *)malloc(sizeof(*array) + max_n * max_m * (sizeof(int)));
    array->max_n = max_n;
    array->max_m = max_m;
    return array;
};

void put_val_in_array(int val, int n_idx, int m_idx, IntArray *array) {
    array->data[n_idx][m_idx] = val;
};

void print_range_string(int start_range, int stop_range, char *array) {
    printf("For range %i-%i in the string: ", start_range, stop_range);
    for (int i = start_range; i <= stop_range; i++) {
        printf("%c", array[i]);
    }
};

int main() {

    FILE *input_file;
    input_file = fopen("dummy.txt", "r");
    if (input_file == NULL) {
        printf("The file is not opened.");
        fclose(input_file);
        return EXIT_FAILURE;
    }

    /*Each line ends with 2 characters -> newline/line feed (ASCII 10) and NULL (ASCII 0)  */
    char line[LINE_LENGTH];
    int number_of_lines = 0;

    // start state of the problem
    int accessible_rolls = 0;
    IntArray *rolls_num = create_array(LINE_LENGTH, LINE_LENGTH);
    IntArray *rolls_char = create_array(LINE_LENGTH, LINE_LENGTH);

    // setup of problem variables
    int line_length;
    bool has_reached_eol;

    while (fgets(line, LINE_LENGTH, input_file) != NULL) {

        // reset line interpretation
        line_length = 0;
        has_reached_eol = false;

        // !! assumption -> input is well formatted and has equal line length
        while (has_reached_eol == false) {

            line_length += 1;
            // line end condition
            if ((int)line[line_length] == 10) {
                has_reached_eol = true;
            }
        }
    }

    printf("Total joltage: %lu\n", sum_joltage);

    fclose(input_file);
    return EXIT_SUCCESS;
}
