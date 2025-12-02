#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#define ONE_KB 1024

typedef enum { false, true } bool;
typedef struct Range {
    unsigned long l_low;
    unsigned long l_high;
    unsigned short num_digits;
} Range;

unsigned long max(unsigned long a, unsigned long b) { return a * (a >= b) + b * (a < b); };
unsigned long min(unsigned long a, unsigned long b) { return a * (a <= b) + b * (a > b); };

unsigned int find_num_inval_ids_for_one_range(int start_low, int end_low, int start_high,
                                              int end_high, char *line) {

    int num_digits_low = end_low - start_low;
    int num_digits_high = end_high - start_high;
    printf("Num digits low: %i | Num digits high: %i\n", num_digits_low, num_digits_high);

    unsigned long sum_invalid_ids = 0;

    unsigned long l_low = 0;
    int counter = 0;
    for (int i = start_low; i < end_low; i++) {
        l_low += (unsigned long)(line[i] - '0') * pow(10, num_digits_low - 1 - counter);
        counter++;
    }

    unsigned long l_high = 0;
    counter = 0;
    for (int i = start_high; i < end_high; i++) {
        l_high += (unsigned long)(line[i] - '0') * pow(10, num_digits_high - 1 - counter);
        counter++;
    }

    printf("low: %lu | high: %lu\n", l_low, l_high);

    if (num_digits_high == num_digits_low && num_digits_high % 2 != 0) {
        printf("\n");
        return 0;
    }

    /*Iterate and construct over possible ranges and find invalid ids for each range*/
    for (int i = num_digits_low; i <= num_digits_high; i++) {
        // check for even number of digits
        if (i % 2 == 0) {

            int power_for_half_digits = i >> 1;

            // complete ranges of the current number of digits
            unsigned long min_curr_range = pow(10, i - 1);
            unsigned long max_curr_range = pow(10, i) - 1;

            // adjusted range to take into account our given numbers
            unsigned long start_curr_range = max(min_curr_range, l_low);
            unsigned long end_curr_range = min(max_curr_range, l_high);

            for (unsigned long j = start_curr_range; j <= end_curr_range; j++) {
                unsigned long bigger_half = j / (unsigned long)pow(10, power_for_half_digits);
                unsigned long lower_half = j - bigger_half * pow(10, power_for_half_digits);
                if (bigger_half == lower_half) {
                    sum_invalid_ids += j;
                    printf("For current range, invalid found: %lu \n", j);
                }
            }
        }
    }

    printf("Total %lu\n\n", sum_invalid_ids);
    return sum_invalid_ids;
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
    char line[ONE_KB];

    char *valid_line = fgets(line, ONE_KB, input_file);
    if (valid_line == NULL) {
        printf("Line could not be read");
        fclose(input_file);
        return EXIT_FAILURE;
    }

    // start state of problem
    unsigned long sum_inval_ids = 0;

    // line specific constants
    char range_sep = '-';
    char item_sep = ',';

    // initialization of loop
    bool eol_reached = false;
    int char_counter = 0;
    int next_item_start = 0;

    // storage of item followers
    int start_low_num;
    int end_low_num;
    int start_high_num;
    int end_high_num;

    while (eol_reached == false) {

        start_low_num = next_item_start;

        char current_char = line[char_counter];

        if (current_char == range_sep) {
            end_low_num = char_counter;
            start_high_num = char_counter + 1;
        }
        if (current_char == item_sep) {
            end_high_num = char_counter;
            next_item_start = char_counter + 1;
            sum_inval_ids += find_num_inval_ids_for_one_range(start_low_num, end_low_num,
                                                              start_high_num, end_high_num, line);
        }

        // end of line, we have one last range to do
        if (current_char == 10) {
            end_high_num = char_counter;
            sum_inval_ids += find_num_inval_ids_for_one_range(start_low_num, end_low_num,
                                                              start_high_num, end_high_num, line);
            eol_reached = true;
        }

        // read next character
        char_counter++;
    }

    // sum_inval_ids = find_num_inval_ids_for_one_range(22, 32, 33, 43, line);

    printf("Sum of invalid ids: %lu \n", sum_inval_ids);

    fclose(input_file);
    return EXIT_SUCCESS;
}
