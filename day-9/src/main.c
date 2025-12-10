#include "helpers/array.h"
#include "helpers/sorting.h"
#include <assert.h>
#include <limits.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#define LINE_LENGTH 256
#define ARRAY_SIZE_N 1024
#define ARRAY_SIZE_M 2

typedef enum { false, true } bool;
typedef struct cardinal_dir {
    int i;
    int j;
} cardinal_dir;

typedef struct Bounday {
    int start_x;
    int stop_x;
    int start_y;
    int stop_y;
    bool vertical;
} Boundary;

typedef struct Segment {
    int start_x;
    int stop_x;
    int start_y;
    int stop_y;
    bool vertical;
} Segment;

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

long absolute_l(long a) { return (a >= 0) * a - (a < 0) * a; };

void get_ij_from_pascal_idx(int pascal_idx, int i_start, int *i, int *j) {
    bool is_ij_found = false;

    if (pascal_idx < i_start * (i_start + 1) / 2) {
        i_start = 0;
    }

    while (is_ij_found == false) {
        int lower_b = i_start * (i_start + 1) / 2;
        int higher_b = (i_start + 1) * (i_start + 2) / 2;

        if (lower_b <= pascal_idx && pascal_idx < higher_b) {
            *i = i_start;
            *j = pascal_idx - lower_b;
            is_ij_found = true;
        }

        i_start++;
    }
}

// rotates a direction vector
void rotate_dir_90_clockwise(cardinal_dir *direction) {
    // in rotation matrix, a = cos (90), b=sin(90), c = -sin(90), d = cos(90)
    int new_x = -direction->j;
    int new_y = direction->i;
    direction->i = new_x;
    direction->j = new_y;
};

void rotate_dir_90_anticlockwise(cardinal_dir *direction) {
    // in rotation matrix, a = cos (90), b=-sin(90), c = sin(90), d = cos(90)
    int new_x = direction->j;
    int new_y = -direction->i;
    direction->i = new_x;
    direction->j = new_y;
};

void normalize_dir(cardinal_dir *direction) {
    direction->i = min_i(1, max_i(-1, direction->i));
    direction->j = min_i(1, max_i(-1, direction->j));
};

void increment_turn_counter(Boundary prev, Boundary current, int *turn_counter) {
    int dx1 = prev.stop_x - prev.start_x;
    int dx2 = current.stop_x - current.start_x;
    int dy1 = prev.stop_y - prev.start_y;
    int dy2 = current.stop_y - current.start_y;

    // right turn
    if ((dx1 > 0 && dy2 > 0) || (dx1 < 0 && dy2 < 0) || (dy1 > 0 && dx2 < 0) ||
        (dy1 < 0 && dx2 > 0)) {
        *turn_counter += 1;
    } else {
        *turn_counter -= 1;
    }
};

bool bound_seg_intersect(Boundary b, Segment s) {
    bool intersects = false;

    int dx_b = absolute_i(b.stop_x - b.start_x);
    int dx_s = absolute_i(s.stop_x - s.start_x);
    int dy_b = absolute_i(b.stop_y - b.start_y);
    int dy_s = absolute_i(s.stop_y - s.start_y);

    // parrallel we are not checking
    if ((dx_b == 0 && dx_s == 0) || (dy_b == 0 && dy_s == 0)) {
        return false;
    }

    // intersection with ends included

    int x_low;
    int x_high;
    int y_low;
    int y_high;
    int x_vl;
    int y_hl;

    // finding correct limits of the vertical and horizontal segments
    if (dx_b > 0 && dx_s == 0) {
        assert(s.start_x == s.stop_x);
        x_low = min_i(b.start_x, b.stop_x);
        x_high = max_i(b.start_x, b.stop_x);
        y_hl = b.start_y;
    } else {
        assert(b.start_x == b.stop_x);
        x_low = min_i(s.start_x, s.stop_x);
        x_high = max_i(s.start_x, s.stop_x);
        y_hl = s.start_y;
    }

    if (dy_b > 0 && dy_s == 0) {
        assert(s.start_y == s.stop_y);
        y_low = min_i(b.start_y, b.stop_y);
        y_high = max_i(b.start_y, b.stop_y);
        x_vl = b.start_x;
    } else {
        assert(b.start_y == b.stop_y);
        y_low = min_i(s.start_y, s.stop_y);
        y_high = max_i(s.start_y, s.stop_y);
        x_vl = s.start_x;
    }

    if ((y_low <= y_hl && y_hl <= y_high) && (x_low <= x_vl && x_vl <= x_high)) {
        intersects = true;
    }

    return intersects;
};

bool is_point_pair_valid(int x1, int y1, int x2, int y2, Boundary *boundaries,
                         int num_boundaries) {

    bool valid = true;

    int dx = absolute_i(x1 - x2);
    int dy = absolute_i(y1 - y2);

    // condition of same point or line
    if (dx == 0 && dy == 0) {
        return true;
    }

    // !!! scaling down the rectangles so that they can never intersect if they are valid
    int low_x = min_i(x1, x2) + 1;
    int low_y = min_i(y1, y2) + 1;

    // some strange math for the correct segment in the following loop
    for (int i = 0; i < 4; i++) {

        // scale down the rectangle by 1 height on each side and 1 width on each side
        // d_ - 2 is because we need to do d_ - 1 for the correct index
        Segment s = {.start_x = low_x + (i % 2) * (dx - 2),
                     .stop_x = low_x + (i > 2) * (dx - 2),
                     .start_y = low_y + ((i + 1) % 2) * (dy - 2),
                     .stop_y = low_y + (i > 2) * (dy - 2),
                     .vertical = (bool)(dx == 0)};

        for (int j = 0; j < num_boundaries; j++) {

            bool intersects = bound_seg_intersect(boundaries[j], s);
            if (intersects == true) {
                return false;
            }
        }
    }

    return valid;
};

void draw_boundaries(Boundary *boundaries, int num_boundaries) {
    IntArray *arr = create_array_i(20, 20);
    fill_array_i(0, arr);
    for (int i = 0; i < num_boundaries; i++) {
        int sx = min_i(boundaries[i].start_x, boundaries[i].stop_x);
        int ex = max_i(boundaries[i].start_x, boundaries[i].stop_x);
        int sy = min_i(boundaries[i].start_y, boundaries[i].stop_y);
        int ey = max_i(boundaries[i].start_y, boundaries[i].stop_y);
        printf("Boundary at i=%i is: x:%i-%i y:%i-%i\n", i, sx, ex, sy, ey);
        for (int k = sy; k <= ey; k++) {
            for (int j = sx; j <= ex; j++) {
                put_val_in_array_i(1, k, j, arr);
            }
        }
    }

    printf("Boundaries: \n");
    print_2d_int_array(arr);
    printf("\n");
};

int main() {

    /*
     * FILE PROCESSING
     */

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
    IntArray *positions = create_array_i(ARRAY_SIZE_N, ARRAY_SIZE_M);
    fill_array_i(0, positions);

    // setup of problem variables
    int line_counter = 0;

    // counting turns of the boundary
    Boundary *boundaries = malloc(ARRAY_SIZE_N * sizeof(boundaries[0]));
    int num_clockwise_turns = 0;

    while (fgets(line, LINE_LENGTH, input_file) != NULL) {

        // reset line interpretation
        bool has_reached_eol = false;
        int in_line_idx = 0;
        int number_length = 0;
        char num_buffer[16];
        int comma_counter = 0;

        // !! assumption -> input is well formatted
        while (has_reached_eol == false) {

            /*
             * NUMBER READING
             */
            // add to the number buffer if it is a number
            if ((int)'0' <= line[in_line_idx] && line[in_line_idx] <= '9') {
                num_buffer[number_length] = line[in_line_idx];
                number_length++;
            }

            if (line[in_line_idx + 1] == ',' || (int)line[in_line_idx + 1] == 10) {
                int val = construct_int_from_chars(number_length, num_buffer);
                put_val_in_array_i(val, line_counter, comma_counter, positions);
                comma_counter++;
                number_length = 0;
            }

            /*
             * END OF LINE
             */

            in_line_idx++;
            // line end condition
            if ((int)line[in_line_idx] == 10) {
                has_reached_eol = true;
            }
        }

        /*
         * Construction of the boundaries and count turning
         */
        if (line_counter == 0) {
            /*Cannot construct a boundary with 1 point */
        } else {
            int dx = absolute_i(positions->data[line_counter - 1][0] -
                                positions->data[line_counter][0]);
            Boundary curr_b = {.start_x = positions->data[line_counter - 1][0],
                               .stop_x = positions->data[line_counter][0],
                               .start_y = positions->data[line_counter - 1][1],
                               .stop_y = positions->data[line_counter][1],
                               .vertical = (bool)(dx == 0)};
            boundaries[line_counter - 1] = curr_b;
        }

        line_counter++;
    }
    /* At end, still need to construct boundary between last and 1st point */

    Boundary curr_b = {
        .start_x = positions->data[line_counter - 1][0],
        .stop_x = positions->data[0][0],
        .start_y = positions->data[line_counter - 1][1],
        .stop_y = positions->data[0][1],
    };
    boundaries[line_counter - 1] = curr_b;
    // draw_boundaries(boundaries, line_counter);
    /*
     * PROBLEM LOGIC
     */

    // Calculate pairwise distances
    int num_pairs = line_counter * (line_counter + 1) / 2;
    long *pairwise_area = (long *)malloc(num_pairs * sizeof(pairwise_area[0]));
    int *area_idxs = (int *)malloc(num_pairs * sizeof(area_idxs[0]));

    // Possible optimization: check rectangle only if area is bigger than local max

    for (int i = 0; i < line_counter; i++) {
        for (int j = 0; j <= i; j++) {
            // if no connection already exist
            long dx = absolute_l(positions->data[i][0] - positions->data[j][0]) + 1;
            long dy = absolute_l(positions->data[i][1] - positions->data[j][1]) + 1;
            long area = dx * dy;
            if (is_point_pair_valid(positions->data[i][0], positions->data[i][1],
                                    positions->data[j][0], positions->data[j][1], boundaries,
                                    line_counter + 1)) {
                pairwise_area[i * (i + 1) / 2 + j] = area;
            } else {
                pairwise_area[i * (i + 1) / 2 + j] = 0;
            }

            area_idxs[i * (i + 1) / 2 + j] = i * (i + 1) / 2 + j;
        }
    }

    // !! the actual index of the last element is num_pairs - 1, not num_pairs
    quick_sort_l(pairwise_area, area_idxs, 0, num_pairs - 1);
    printf("Largest area is %li \n", pairwise_area[num_pairs - 1]);

    // memory management
    free(pairwise_area);
    free(area_idxs);
    free(boundaries);
    free_array_i(positions);

    fclose(input_file);
    return EXIT_SUCCESS;
}
