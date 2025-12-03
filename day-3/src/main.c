#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#define LINE_LENGTH 128

typedef enum { false, true } bool;

char max(char a, char b) { return a * (a >= b) + b * (a < b); };

void print_range_string(int start_range, int stop_range, char *array) {
    printf("For range %i-%i in the string: ", start_range, stop_range);
    for (int i = start_range; i <= stop_range; i++) {
        printf("%c", array[i]);
    }
};

char max_within_range(int start_range, int end_range, int *max_position, char *line) {
    char max_char = '0';

    for (int i = start_range; i <= end_range; i++) {
        if (line[i] > max_char) {
            max_char = line[i];
            *max_position = i;
        }
    }

    /*
    print_range_string(start_range, end_range, line);
    printf(" max character is: %c\n", max_char);
     */

    return max_char;
};

unsigned long construct_long_from_chars(int length_num, char *chars) {
    unsigned long result = 0;

    /*
    print_range_string(0, length_num, chars);
    printf(" is used to construct long\n");
    */

    for (int i = 1; i <= length_num; i++) {
        result += (chars[i - 1] - '0') * (unsigned long)pow(10, length_num - i);
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
    unsigned long sum_joltage = 0;
    int num_char_to_search = 12;
    char local_joltage_buffer[num_char_to_search];

    // setup of problem variables
    int line_length;
    bool has_reached_eol;

    while (fgets(line, LINE_LENGTH, input_file) != NULL) {

        // reset line interpretation
        line_length = 0;
        has_reached_eol = false;

        while (has_reached_eol == false) {
            line_length += 1;
            // line end condition
            if ((int)line[line_length] == 10) {
                has_reached_eol = true;
            }
        }

        int loc_previous_char = 0;
        for (int i = 0; i < num_char_to_search; i++) {
            int end_current_range = line_length - num_char_to_search + i;
            int start_curr_range = loc_previous_char;
            local_joltage_buffer[i] =
                max_within_range(start_curr_range, end_current_range, &loc_previous_char, line);
            loc_previous_char += 1;
        }

        unsigned long line_joltage =
            construct_long_from_chars(num_char_to_search, local_joltage_buffer);
        sum_joltage += line_joltage;
        printf("Joltage is: %lu for line %s\n", line_joltage, line);
    }

    printf("Total joltage: %lu\n", sum_joltage);

    fclose(input_file);
    return EXIT_SUCCESS;
}
