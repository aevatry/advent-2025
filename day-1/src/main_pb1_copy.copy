#include <math.h>
#include <stdio.h>
#include <stdlib.h>

typedef enum { false, true } bool;

int main() {

    FILE *input_file;
    input_file = fopen("input_pb1.txt", "r");
    if (input_file == NULL) {
        printf("The file is not opened.");
        fclose(input_file);
        return EXIT_FAILURE;
    }

    // state of problem
    int current_dial = 50;
    int password = 0;

    // setup of needed variables
    int curr_instruction;
    int sign;
    bool has_reached_eol;
    int num_digits;
    /*Each line ends with 2 characters -> newline/line feed (ASCII 10) and NULL (ASCII 0)  */
    char line[50];

    while (fgets(line, 50, input_file) != NULL) {

        // left or right is incrementing direction
        char letter = line[0];
        if (letter == 'R') {
            sign = 1;
        } else {
            sign = -1;
        }

        num_digits = 0;
        has_reached_eol = false;
        while (has_reached_eol == false) {
            num_digits += 1;
            if ((int)line[num_digits + 1] == 10) {
                has_reached_eol = true;
            }
        }

        curr_instruction = 0;
        for (int i = 0; i < num_digits; i++) {
            curr_instruction += sign * ((int)line[i + 1] - '0') * (int)pow(10, num_digits - 1 - i);
        }

        // logic for counting password
        current_dial = (current_dial + curr_instruction) % 100;
        if (current_dial < 0) {
            current_dial += 100;
        }

        // password increment
        if (current_dial == 0) {
            password += 1;
        }
    }

    printf("Final password: %i \n", password);

    fclose(input_file);
    return EXIT_SUCCESS;
}
