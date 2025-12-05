#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#define LINE_LENGTH 128

typedef enum { false, true } bool;

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
    int fresh_total = 0;
    int current_write_bound_idx = 0;
    unsigned long low_bound[256];
    unsigned long high_bound[256];

    // setup of problem variables
    bool has_reached_eol;
    bool has_reached_separation = false;

    while (fgets(line, LINE_LENGTH, input_file) != NULL) {

        // check for separation
        if ((int)line[0] == 10) {
            has_reached_separation = true;
            /* Line of no interest we just need to change processing mode */
            continue;
        };

        // reset line interpretation
        int line_idx = 0;
        has_reached_eol = false;

        while (has_reached_eol == false) {

            line_idx += 1;
            // line end condition
            if ((int)line[line_idx] == 10) {
                has_reached_eol = true;
            }
        }

        if (has_reached_separation == false) {
            process_range_line(low_bound, high_bound, &current_write_bound_idx, line);
        } else {
            unsigned long id = process_id_line(line);
            fresh_total += is_id_fresh(low_bound, high_bound, current_write_bound_idx, id);
        }
    }

    printf("Number of fresh items: %i\n", fresh_total);

    fclose(input_file);
    return EXIT_SUCCESS;
}
