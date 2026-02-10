#include "helpers/array.h"
#include "helpers/list.h"
#include "helpers/sorting.h"
#include <assert.h>
#include <limits.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define LINE_LENGTH 256

typedef enum { false, true } bool;

typedef struct JoltageState {
    IntList *objective;
    IntListOL *buttons;
    IntList *current_state;
    IntList *used_buttons;
    long num_calls;
    int minimum_found_depth;
} JoltageState;

typedef void (*Update)(JoltageState *state, int idx, int scaler);

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

void update_template(IntList *state, IntList *button, int scaler) {
    for (int i = 0; i < button->idx_next_elem; i++) {
        int idx_given_by_button = button->data[i];
        assert(idx_given_by_button < state->idx_next_elem);
        state->data[idx_given_by_button] = (state->data[idx_given_by_button] + scaler) % 2;
    }
}

void update_joltage_fwd(JoltageState *jolt_state, int button_idx, int scaler) {
    assert(scaler >= 0);
    IntList *button = get_sub_list(button_idx, jolt_state->buttons);
    IntList *state = jolt_state->current_state;
    for (int i = 0; i < button->idx_next_elem; i++) {
        int idx_given_by_button = button->data[i];
        state->data[idx_given_by_button] = (state->data[idx_given_by_button] + scaler);
    }
    jolt_state->used_buttons->data[button_idx] = 1;
}
void update_joltage_bwd(JoltageState *jolt_state, int button_idx, int scaler) {
    assert(scaler >= 0);
    IntList *button = get_sub_list(button_idx, jolt_state->buttons);
    IntList *state = jolt_state->current_state;
    for (int i = 0; i < button->idx_next_elem; i++) {
        int idx_given_by_button = button->data[i];
        state->data[idx_given_by_button] = (state->data[idx_given_by_button] - scaler);
    }
    jolt_state->used_buttons->data[button_idx] = 0;
}

int pick_best_button(IntList *objective, IntListOL *buttons, IntList *current_state,
                     IntList *used_buttons, int *max_num_presses) {
    int idx_best_button = -1;
    int max_closing = 0;

    for (int i = 0; i < buttons->num_sublists; i++) {
        IntList *curr_button = get_sub_list(i, buttons);
        int closing = -1;

        // if button unused we can proceed
        if (used_buttons->data[i] == 0) {
            closing = curr_button->idx_next_elem;
            for (int j = 0; j < curr_button->idx_next_elem; j++) {
                int button_idx = curr_button->data[j];
                int delta = objective->data[button_idx] - (current_state->data[button_idx] + 1);
                if (delta < 0) {
                    closing = -1;
                }
            }
        }

        if (closing >= 0 && closing > max_closing) {
            idx_best_button = i;
            max_closing = closing;
        }
    }

    // control flow if there is no valid button
    if (idx_best_button < 0) {
        return -1;
    }

    int min_presses_to_target = INT_MAX;

    IntList *best_button = get_sub_list(idx_best_button, buttons);
    for (int j = 0; j < best_button->idx_next_elem; j++) {
        int button_idx = best_button->data[j];
        int curr_presses = objective->data[button_idx] - current_state->data[button_idx];
        if (curr_presses < min_presses_to_target) {
            min_presses_to_target = curr_presses;
        }
    }
    *max_num_presses = min_presses_to_target;

    return idx_best_button;
}

long absolute_l(long a) { return (a >= 0) * a - (a < 0) * a; };

void sort_buttons_euristic(IntList *objective, IntListOL *buttons, IntList *current_state,
                           int *indices) {

    int distances[buttons->num_sublists];

    // This the wrong way to calculate distances
    for (int i = 0; i < buttons->num_sublists; i++) {
        IntList *curr_button = get_sub_list(i, buttons);
        distances[i] = 0;
        indices[i] = i;
        for (int j = 0; j < curr_button->idx_next_elem; j++) {
            int button_idx = curr_button->data[j];
            int local_diff = objective->data[button_idx] - current_state->data[button_idx] - 1;
            distances[i] += local_diff;
            if (local_diff < 0) {
                distances[i] += 10000;
            }
        }
    }

    quick_sort_i(distances, indices, 0, buttons->num_sublists - 1);
}

bool has_state_overstepped(IntList *objective, IntList *current_state) {
    bool has_overstepped = false;

    for (int i = 0; i < objective->idx_next_elem; i++) {
        if (current_state->data[i] > objective->data[i]) {
            has_overstepped = true;
        }
    }

    return has_overstepped;
}

bool will_not_be_min(IntList *objective, IntList *current_state, int minimum_found_depth,
                     int current_depth) {
    bool will_not_be_min = false;
    int delta_to_close = minimum_found_depth - current_depth;
    for (int i = 0; i < objective->idx_next_elem; i++) {
        int current_delta = objective->data[i] - current_state->data[i];
        if (current_delta > delta_to_close) {
            will_not_be_min = true;
        }
    }

    return will_not_be_min;
}

int minimum_depth_for_constraint(JoltageState *state, int curr_depth, int idx_button_back_upd,
                                 int scaler, Update forward_upd, Update backward_upd) {

    state->num_calls += 1;

    // never search deeper than an existing solution
    if (curr_depth >= state->minimum_found_depth) {
        backward_upd(state, idx_button_back_upd, scaler);
        return curr_depth;
    }

    // if we have overstepped, return negative
    if (has_state_overstepped(state->objective, state->current_state) == true) {
        backward_upd(state, idx_button_back_upd, scaler);
        return -1;
    }

    if (will_not_be_min(state->objective, state->current_state, state->minimum_found_depth,
                        curr_depth) == true) {
        backward_upd(state, idx_button_back_upd, scaler);
        return state->minimum_found_depth + 1;
    }

    // if we have a match, return the current depth
    if (is_list_i_strictly_equal(state->objective, state->current_state) == 1) {
        backward_upd(state, idx_button_back_upd, scaler);
        return curr_depth;
    }

    int max_scaler;
    int idx_best_button = pick_best_button(state->objective, state->buttons, state->current_state,
                                           state->used_buttons, &max_scaler);

    // return if no button available
    if (idx_best_button < 0) {
        backward_upd(state, idx_button_back_upd, scaler);
        return -1;
    }

    for (int new_scale = max_scaler; new_scale >= 0; new_scale--) {
        // pick button and update to correct scale
        forward_upd(state, idx_best_button, new_scale);

        // recursion step
        int depth_reached = minimum_depth_for_constraint(
            state, curr_depth + new_scale, idx_best_button, new_scale, forward_upd, backward_upd);

        // Update conditions for the depth
        if (depth_reached > 0 && depth_reached < state->minimum_found_depth) {
            state->minimum_found_depth = depth_reached;
        }
    }

    // Now that we have the minimum, update state backward (if not at start node) and
    // return found minimum
    if (curr_depth > 0) {
        backward_upd(state, idx_button_back_upd, scaler);
    }

    return state->minimum_found_depth;
};

int main() {

    clock_t begin = clock();

    /*
     * FILE PROCESSING
     */

    FILE *input_file;
    input_file = fopen("small_input.txt", "r");
    if (input_file == NULL) {
        printf("The file is not opened.");
        fclose(input_file);
        return EXIT_FAILURE;
    }

    /*Each line ends with 2 characters -> newline/line feed (ASCII 10) and NULL (ASCII 0)
     */
    char line[LINE_LENGTH];
    unsigned long total_sum = 0;

    while (fgets(line, LINE_LENGTH, input_file) != NULL) {

        // reset line interpretation
        bool has_reached_eol = false;
        int in_line_idx = 0;

        // number reading
        int number_length = 0;
        char num_buffer[16];

        // For all different modes
        bool in_template_mode = false;
        char entry_template = '[';
        char exit_template = ']';
        bool in_joltage_mode = false;
        char entry_joltage = '{';
        char exit_joltage = '}';
        bool in_button_mode = false;
        char entry_button = '(';
        char exit_button = ')';

        // input lists to our problem
        IntList *template = create_list_i(8);
        IntList *state = create_list_i(8);
        IntList *joltages = create_list_i(8);
        IntListOL *buttons = create_listol_i();
        IntList *used_buttons = create_list_i(8);

        // !! assumption -> input is well formatted
        while (has_reached_eol == false) {
            /*
             * SCENARIO READING
             */

            // number reading
            if ((int)'0' <= line[in_line_idx] && line[in_line_idx] <= '9') {
                num_buffer[number_length] = line[in_line_idx];
                number_length++;
            }

            if (line[in_line_idx] == entry_template) {
                in_template_mode = true;
            }
            if (line[in_line_idx] == exit_template) {
                in_template_mode = false;
            }

            if (line[in_line_idx] == entry_button) {
                in_button_mode = true;
                int size_sub_list = 12;
                add_sub_list_to_listol(size_sub_list, buttons);
                push_val_in_list_i(0, used_buttons);
            }
            if (line[in_line_idx] == exit_button) {
                in_button_mode = false;
            }
            if (line[in_line_idx] == entry_joltage) {
                in_joltage_mode = true;
            }
            if (line[in_line_idx] == exit_joltage) {
                in_joltage_mode = false;
            }

            /*
             * NUMBER PROCESSING
             */

            if (in_template_mode == true && line[in_line_idx] != entry_template) {
                if (line[in_line_idx] == '.') {
                    push_val_in_list_i(0, template);
                } else {
                    push_val_in_list_i(1, template);
                }
                push_val_in_list_i(0, state);
            }

            if (line[in_line_idx + 1] == ',' || line[in_line_idx + 1] == exit_button ||
                line[in_line_idx + 1] == exit_joltage) {

                int val = construct_int_from_chars(number_length, num_buffer);
                number_length = 0;

                if (in_button_mode == true) {
                    push_val_in_listol_i(val, buttons->num_sublists - 1, buttons);
                }

                if (in_joltage_mode == true) {
                    push_val_in_list_i(val, joltages);
                }
            }

            /*
             * END OF LINE CHECKING
             */

            in_line_idx++;
            // line end condition. Assume all lines well conditionned
            if ((int)line[in_line_idx] == 10) {
                has_reached_eol = true;
            }
        }

        printf("Template list: ");
        print_list(template);

        printf("Joltage list: ");
        print_list(joltages);

        printf("Buttons: \n");
        for (int i = 0; i < buttons->num_sublists; i++) {
            print_list(buttons->all_lists[i]);
        }

        JoltageState jolt_state = {.current_state = state,
                                   .objective = joltages,
                                   .buttons = buttons,
                                   .used_buttons = used_buttons,
                                   .minimum_found_depth = INT_MAX,
                                   .num_calls = 0};
        int min_num_steps = minimum_depth_for_constraint(&jolt_state, 0, 0, 1, update_joltage_fwd,
                                                         update_joltage_bwd);
        total_sum += min_num_steps;
        printf("Minimum number of steps found is: %d with %li steps\n\n", min_num_steps,
               jolt_state.num_calls);

        free_list_i(template);
        free_list_i(joltages);
        free_listol_i(buttons);
    }
    /* At end, still need to construct boundary between last and 1st point */

    printf("Total sum is: %lu\n", total_sum);

    clock_t end = clock();
    printf("Elapsed: %f seconds\n", (double)(end - begin) / CLOCKS_PER_SEC);

    fclose(input_file);
    return EXIT_SUCCESS;
}
