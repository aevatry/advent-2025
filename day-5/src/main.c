#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define LINE_LENGTH 128

typedef enum { false, true } bool;

void swap_long(unsigned long *a, unsigned long *b) {
    unsigned long temp = *a;
    *a = *b;
    *b = temp;
}
void swap_int(int *a, int *b) {
    int temp = *a;
    *a = *b;
    *b = temp;
}

// Partition function (as defined above)
int partition(unsigned long *arr, int *indices, int low, int high) {
    unsigned long pivot = arr[high];
    int i = low;
    for (int j = low; j < high; j++) {
        if (arr[j] <= pivot) {
            swap_long(&arr[i], &arr[j]);
            swap_int(&indices[i], &indices[j]);
            i++;
        }
    }
    swap_long(&arr[i], &arr[high]);
    swap_int(&indices[i], &indices[high]);
    return i;
}

// Quick Sort function (as defined above)
void quickSort(unsigned long *arr, int *indices, int low, int high) {
    if (low < high) {
        int pi = partition(arr, indices, low, high);
        quickSort(arr, indices, low, pi - 1);
        quickSort(arr, indices, pi + 1, high);
    }
}

// Utility function to print the array
void printIntArray(int *arr, int size) {
    for (int i = 0; i < size; i++) {
        printf("%d ", arr[i]);
    }
    printf("\n");
}
void printULongArray(unsigned long *arr, int size) {
    for (int i = 0; i < size; i++) {
        printf("%lu ", arr[i]);
    }
    printf("\n");
}

unsigned long construct_long_from_chars(int length_num, char *chars) {
    unsigned long result = 0;

    for (int i = 1; i <= length_num; i++) {
        result += (chars[i - 1] - '0') * (unsigned long)pow(10, length_num - i);
    }
    return result;
};

void process_range_line(unsigned long *low_bound, unsigned long *high_bound, int *prev_idx,
                        char *line) {

    int length_number = 0;
    int line_idx = 0;
    while (line[line_idx] != 10) {
        line_idx += 1;
        length_number += 1;
        if (line[line_idx] == '-') {
            unsigned long low =
                construct_long_from_chars(length_number, &line[line_idx - length_number]);
            low_bound[*prev_idx] = low;
            // reset length of currently read number: "6-10" we want "1" at line_idx = 2, lenght=0
            // (not 1)
            length_number = -1;
        }

        if (line[line_idx] == 10) {
            // line_idx-1 needed because the '-' counts towards the line idx but not accounted in
            // length_num
            unsigned long high =
                construct_long_from_chars(length_number, &line[line_idx - length_number]);
            high_bound[*prev_idx] = high;
        }
    }
    *prev_idx += 1;
};

unsigned long process_id_line(char *line) {
    unsigned long id;

    int line_idx = 0;
    while (line[line_idx] != 10) {
        line_idx += 1;
        if (line[line_idx] == 10) {
            id = construct_long_from_chars(line_idx, line);
        }
    }
    return id;
};

int is_id_fresh(unsigned long *low_bound, unsigned long *high_bound, int last_writable_idx,
                unsigned long curr_id) {
    int num_ranges_fall = 0;

    for (int i = 0; i < last_writable_idx; i++) {
        if (low_bound[i] <= curr_id && curr_id <= high_bound[i]) {
            num_ranges_fall += 1;
        }
    }

    return num_ranges_fall > 0;
};

unsigned long tot_range_count(unsigned long *low_bounds, unsigned long *high_bounds,
                              int last_writable_idx) {
    unsigned long res = 0;

    int index_used[last_writable_idx];
    int indices[last_writable_idx];
    for (int i = 0; i < last_writable_idx; i++) {
        indices[i] = i;
        index_used[i] = 0;
    }

    quickSort(low_bounds, indices, 0, last_writable_idx - 1);

    for (int i = 0; i < last_writable_idx; i++) {

        if (index_used[i] == 0) {
            int curr_max_idx = indices[i];
            unsigned long curr_min = low_bounds[i];
            unsigned long curr_max = high_bounds[curr_max_idx];

            for (int j = i + 1; j < last_writable_idx; j++) {
                int temp_max_idx = indices[j];
                if (low_bounds[j] <= curr_max && curr_max <= high_bounds[temp_max_idx]) {
                    curr_max = high_bounds[temp_max_idx];
                    index_used[j] = 1;
                }
                if (low_bounds[j] <= curr_max && high_bounds[temp_max_idx] <= curr_max) {
                    index_used[j] = 1;
                }
            }
            res += curr_max - curr_min + 1;
            printf("Found continuous range: %lu - %lu with range: %lu\n", curr_min, curr_max,
                   curr_max - curr_min + 1);
        }
    }

    return res;
}

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
    int current_write_bound_idx = 0;
    unsigned long low_bounds[256] = {0};
    unsigned long high_bounds[256] = {0};

    // setup of problem variables
    bool has_reached_separation = false;

    while (fgets(line, LINE_LENGTH, input_file) != NULL) {

        // check for separation
        if ((int)line[0] == 10) {
            has_reached_separation = true;
            /* Line of no interest we just need to change processing mode */
            continue;
        };

        if (has_reached_separation == false) {
            process_range_line(low_bounds, high_bounds, &current_write_bound_idx, line);
        }
    }

    unsigned long fresh_tot = tot_range_count(low_bounds, high_bounds, current_write_bound_idx);

    printf("Number of fresh items: %lu\n", fresh_tot);

    fclose(input_file);
    return EXIT_SUCCESS;
}
