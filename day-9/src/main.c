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
    cardinal_dir in_direction;
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

void next_bound_indir_from_previous(Boundary *prev, Boundary *current) {
    int dx1 = prev->stop_x - prev->start_x;
    int dx2 = current->stop_x - current->start_x;
    int dy1 = prev->stop_y - prev->start_y;
    int dy2 = current->stop_y - current->start_y;

    current->in_direction = prev->in_direction;

    // right turn
    if ((dx1 > 0 && dy2 > 0) || (dx1 < 0 && dy2 < 0) || (dy1 > 0 && dx2 < 0) ||
        (dy1 < 0 && dx2 > 0)) {
        rotate_dir_90_clockwise(&current->in_direction);
    } else if ((dx1 > 0 && dy2 < 0) || (dx1 < 0 && dy2 > 0) || (dy1 > 0 && dx2 > 0) ||
               (dy1 < 0 && dx2 < 0)) {
        rotate_dir_90_anticlockwise(&current->in_direction);
    } else {
    }
};

void push_bound_1_back(Boundary *bound) {
    // push back the boundary by 1 unit

    // normalize direction to -1, 0, 1
    normalize_dir(&(bound->in_direction));

    if (bound->vertical == true) {
        bound->start_x -= bound->in_direction.i;
        bound->stop_x -= bound->in_direction.i;
    } else {
        bound->start_y -= bound->in_direction.j;
        bound->stop_y -= bound->in_direction.j;
    }
};

bool bound_seg_intersect(Boundary b, Segment s) {
    bool intersect_and_invalid = false;

    int dx_b = absolute_i(b.stop_x - b.start_x);
    int dx_s = absolute_i(s.stop_x - s.start_x);
    int dy_b = absolute_i(b.stop_y - b.start_y);
    int dy_s = absolute_i(s.stop_y - s.start_y);

    // parrallel and same axis => line out of bounds
    if ((dx_b == 0 && dx_s == 0 && b.start_x == s.start_x) ||
        (dy_b == 0 && dy_s == 0 && b.start_y == s.start_y)) {
        return true;
    }

    int x_low;
    int x_high;
    int y_low;
    int y_high;
    int x_vl;
    int y_hl;

    if (dx_b > 0 && dx_s == 0) {
        x_low = min_i(b.start_x, b.stop_x);
        x_high = max_i(b.start_x, b.stop_x);
        assert(s.start_y = s.stop_y);
        y_hl = s.start_y;
    } else {
        x_low = min_i(s.start_x, s.stop_x);
        x_high = max_i(s.start_x, s.stop_x);
        assert(b.start_y = b.stop_y);
        y_hl = b.start_y;
    }

    if (dy_b > 0 && dy_s == 0) {
        y_low = min_i(b.start_y, b.stop_y);
        y_high = max_i(b.start_y, b.stop_y);
        assert(s.start_x = s.stop_x);
        x_vl = s.start_x;
    } else {
        y_low = min_i(b.start_y, b.stop_y);
        y_high = max_i(b.start_y, b.stop_y);
        assert(s.start_x = s.stop_x);
        x_vl = s.start_x;
    }

    return intersect_and_invalid;
};

bool is_point_pair_valid(int x1, int y1, int x2, int y2, Boundary *boundaries,
                         int num_boundaries) {
    bool valid = false;

    int dx = absolute_i(x1 - x2);
    int dy = absolute_i(y1 - y2);

    // condition of same point
    if (dx == 0 && dy == 0) {
        return true;
    }

    int low_x = min_i(x1, x2);
    int low_y = min_i(y1, y2);

    // some strange math for the correct segment in the following loop
    for (int i = 0; i < 4; i++) {

        Segment s = {.start_x = low_x + (i % 2) * dx,
                     .stop_x = low_x + (i > 2) * dx,
                     .start_y = low_y + ((i + 1) % 2) * dx,
                     .stop_y = low_y + (i > 2) * dy,
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
        } else if (line_counter == 1) {

            int dx = absolute_i(positions->data[line_counter - 1][0] -
                                positions->data[line_counter][0]);
            Boundary curr_b = {.start_x = positions->data[line_counter - 1][0],
                               .stop_x = positions->data[line_counter][0],
                               .start_y = positions->data[line_counter - 1][1],
                               .stop_y = positions->data[line_counter][1],
                               .vertical = (bool)(dx == 0)};
            boundaries[line_counter - 1] = curr_b;
        } else {
            int dx = absolute_i(positions->data[line_counter - 1][0] -
                                positions->data[line_counter][0]);
            Boundary curr_b = {.start_x = positions->data[line_counter - 1][0],
                               .stop_x = positions->data[line_counter][0],
                               .start_y = positions->data[line_counter - 1][1],
                               .stop_y = positions->data[line_counter][1],
                               .vertical = (bool)(dx == 0)};
            boundaries[line_counter - 1] = curr_b;
            increment_turn_counter(boundaries[line_counter - 2], boundaries[line_counter - 1],
                                   &num_clockwise_turns);
        }

        line_counter++;
    }
    /* At end, still need to construct boundary between last and 1st point */

    Boundary curr_b = {
        .start_x = positions->data[line_counter][0],
        .stop_x = positions->data[0][0],
        .start_y = positions->data[line_counter][1],
        .stop_y = positions->data[0][1],
    };
    boundaries[line_counter] = curr_b;
    increment_turn_counter(boundaries[line_counter - 1], boundaries[0], &num_clockwise_turns);

    // give correct direction to first tile
    cardinal_dir dir_first_segment = {.i = boundaries[0].stop_x - boundaries[0].start_x,
                                      .j = boundaries[0].stop_y - boundaries[0].start_y};
    normalize_dir(&dir_first_segment);
    if (num_clockwise_turns == 3) {
        rotate_dir_90_clockwise(&dir_first_segment);
    } else if (num_clockwise_turns == -3) {
        rotate_dir_90_anticlockwise(&dir_first_segment);
    } else {
        printf("Boundaries are not closed, exiting program \n");
        return EXIT_FAILURE;
    }
    boundaries[0].in_direction = dir_first_segment;
    push_bound_1_back(&boundaries[0]);

    /*
     * PROBLEM LOGIC
     */

    // construct pushed boundaries and direction of boundary
    int num_boundaries = line_counter + 1;
    for (int i = 1; i < line_counter + 1; i++) {
        next_bound_indir_from_previous(&boundaries[i - 1], &boundaries[i]);
        push_bound_1_back(&boundaries[i]);
    }

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
            if (is_point_pair_valid(positions->data[i][0], positions->data[j][0],
                                    positions->data[i][1], positions->data[j][1], boundaries,
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
