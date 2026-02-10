#include "helpers/array.h"
#include "helpers/list.h"
#include "helpers/sorting.h"
#include <assert.h>
#include <limits.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#define LINE_LENGTH 256
#define DEV_LEN 3

typedef enum { false, true } bool;
typedef struct {
    char name[DEV_LEN];
} device;

typedef struct DevLink {
    device *source_dev;
    device destination_devs[];
} DevLink;

int is_device_source_dev(device *dev, DevLink *kv_pair) {
    int are_keys_equal = 1;

    for (int i = 0; i < DEV_LEN; i++) {
        are_keys_equal &= dev->name[i] && kv_pair->source_dev->name[i];
    }

    return are_keys_equal;
};

int absolute_i(int a) { return (a >= 0) * a - (a < 0) * a; };

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

void update_template(IntList *state, IntList *button) {
    for (int i = 0; i < button->idx_next_elem; i++) {
        int idx_given_by_button = button->data[i];
        assert(idx_given_by_button < state->idx_next_elem);
        state->data[idx_given_by_button] = (state->data[idx_given_by_button] + 1) % 2;
    }
}

long absolute_l(long a) { return (a >= 0) * a - (a < 0) * a; };

int main() {

    /*
     * FILE PROCESSING
     */

    FILE *input_file;
    input_file = fopen("dummy.txt", "r");
    if (input_file == NULL) {
        printf("The file is not opened.");
        fclose(input_file);
        return EXIT_FAILURE;
    }

    /*Each line ends with 2 characters -> newline/line feed (ASCII 10) and NULL (ASCII 0)  */
    char line[LINE_LENGTH];
    unsigned long total_sum = 0;

    while (fgets(line, LINE_LENGTH, input_file) != NULL) {

        // reset line interpretation
        bool has_reached_eol = false;
        int in_line_idx = 0;

        // !! assumption -> input is well formatted
        while (has_reached_eol == false) {
            /*
             * SCENARIO READING
             */

            /*
             * END OF LINE CHECKING
             */

            in_line_idx++;
            // line end condition. Assume all lines well conditionned
            if ((int)line[in_line_idx] == 10) {
                has_reached_eol = true;
            }
        }
    }

    printf("Total sum is: %lu\n", total_sum);

    fclose(input_file);
    return EXIT_SUCCESS;
}
