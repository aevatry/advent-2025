#include "helpers/array.h"
#include "helpers/sorting.h"
#include <assert.h>
#include <limits.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#define LINE_LENGTH 256
#define ARRAY_SIZE_N 1024
#define ARRAY_SIZE_M 3

typedef enum { false, true } bool;

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

int sum_line(IntArray *adj, int n_idx) {
    int res = 0;
    for (int j = 0; j <= adj->m; j++) {
        res += adj->data[n_idx][j];
    }
    return res;
}

void find_neighbors(IntArray *adj, int num_neighbors, int neighbors[num_neighbors],
                    int start_idx) {
    int neighbor_idx = 0;
    for (int j = 0; j <= adj->m; j++) {
        if (adj->data[start_idx][j] == 1) {
            assert(neighbor_idx < num_neighbors);
            neighbors[neighbor_idx] = j;
            neighbor_idx++;
        }
    }
};

bool has_unexplored_neighbors(int num_neighbors, int neighbors[num_neighbors],
                              int *touched_nodes) {
    bool has_unexplored_neighbors = false;

    for (int i = 0; i < num_neighbors; i++) {
        int neighbor_idx = neighbors[i];
        if (touched_nodes[neighbor_idx] == 0) {
            has_unexplored_neighbors = true;
        }
    }

    return has_unexplored_neighbors;
};

int find_graph_from_idx(IntArray *adj, int *touched_nodes, int num_nodes, int start_idx) {
    int size_graph = 1;

    int sub_graph_id = touched_nodes[start_idx];

    int num_neighbors = sum_line(adj, start_idx);
    int neighbors[num_neighbors];

    find_neighbors(adj, num_neighbors, neighbors, start_idx);

    if (has_unexplored_neighbors(num_neighbors, neighbors, touched_nodes) == false) {
        size_graph = 1;
    } else {
        for (int i = 0; i < num_neighbors; i++) {
            int direct_neighbor_idx = neighbors[i];
            if (touched_nodes[direct_neighbor_idx] == 0) {
                touched_nodes[direct_neighbor_idx] = sub_graph_id;
                size_graph +=
                    find_graph_from_idx(adj, touched_nodes, num_nodes, direct_neighbor_idx);
            }
        }
    }

    return size_graph;
};

void find_subgraphs(IntArray *adj, int *subgraphs_size, int *num_subgraphs, int num_nodes) {

    int touched_nodes[num_nodes];
    int subgraph_id = 2;
    *num_subgraphs = 0;
    for (int i = 0; i < num_nodes; i++) {
        if (adj->data[i][i] == 1) {
            touched_nodes[i] = 1;
        } else {
            touched_nodes[i] = 0;
        }
    }

    for (int i = 0; i < num_nodes; i++) {
        if (touched_nodes[i] == 0) {

            // initialise the subgraph at first node encountered.
            touched_nodes[i] = subgraph_id;

            // find the corresponding subgraph
            int size_subgraph = find_graph_from_idx(adj, touched_nodes, num_nodes, i);

            // Fill subgraph array.
            subgraphs_size[subgraph_id - 2] = size_subgraph;

            // next subgraph
            subgraph_id++;
            *num_subgraphs += 1;
        }
    }
};

int sum_diag(IntArray *adj_matrix) {
    int res = 0;
    for (int i = 0; i <= adj_matrix->n; i++) {
        res += adj_matrix->data[i][i];
    }

    return res;
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

    while (fgets(line, LINE_LENGTH, input_file) != NULL) {

        // reset line interpretation
        bool has_reached_eol = false;
        int in_line_idx = 0;
        int number_length = 0;
        char num_buffer[16];
        int comma_counter = 0;

        // !! assumption -> input is well formatted
        while (has_reached_eol == false) {

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

            in_line_idx++;
            // line end condition
            if ((int)line[in_line_idx] == 10) {
                has_reached_eol = true;
            }
        }
        line_counter++;
    }

    /*
     * PROBLEM LOGIC
     */

    // Create adjacency matrix
    IntArray *adj_matrix = create_array_i(line_counter, line_counter);
    fill_array_i(0, adj_matrix);
    for (int i = 0; i < line_counter; i++) {
        put_val_in_array_i(1, i, i, adj_matrix);
    }

    // Calculate pairwise distances
    int num_pairs = line_counter * (line_counter + 1) / 2;
    long *pairwise_distance = (long *)malloc(num_pairs * sizeof(pairwise_distance[0]));
    int *dist_idxs = (int *)malloc(num_pairs * sizeof(dist_idxs[0]));

    for (int i = 0; i < line_counter; i++) {
        for (int j = 0; j <= i; j++) {
            // if no connection already exist
            long dx = positions->data[i][0] - positions->data[j][0];
            long dy = positions->data[i][1] - positions->data[j][1];
            long dz = positions->data[i][2] - positions->data[j][2];
            long poly_dist = dx * dx + dy * dy + dz * dz;
            pairwise_distance[i * (i + 1) / 2 + j] = poly_dist;
            dist_idxs[i * (i + 1) / 2 + j] = i * (i + 1) / 2 + j;
        }
    }

    // fill adjancency matrix
    // !! the actual index of the last element is num_pairs - 1, not num_pairs
    quick_sort_l(pairwise_distance, dist_idxs, 0, num_pairs - 1);

    // reason for starting at num_nodes: d(i, i) = 0. There are num_nodes such connection
    int curr_dist_to_consider = line_counter;

    while (sum_diag(adj_matrix) > 1 && curr_dist_to_consider < num_pairs) {
        int full_idx = dist_idxs[curr_dist_to_consider];
        int i_idx;
        int j_idx;

        get_ij_from_pascal_idx(full_idx, 0, &i_idx, &j_idx);

        put_val_in_array_i(0, i_idx, i_idx, adj_matrix);
        put_val_in_array_i(0, j_idx, j_idx, adj_matrix);

        put_val_in_array_i(1, i_idx, j_idx, adj_matrix);
        put_val_in_array_i(1, j_idx, i_idx, adj_matrix);

        curr_dist_to_consider += 1;
    }

    // find subgraphs and size of those
    int size_subgraphs[line_counter];
    for (int i = 0; i < line_counter; i++) {
        size_subgraphs[i] = 0;
    }
    int num_big_subgraphs;

    find_subgraphs(adj_matrix, size_subgraphs, &num_big_subgraphs, line_counter);

    int indices_subgraphs[num_big_subgraphs];
    for (int i = 0; i < num_big_subgraphs; i++) {
        indices_subgraphs[i] = i;
        printf("Subgraph found of size: %i\n", size_subgraphs[i]);
    }

    if (num_big_subgraphs == 1) {
        bool correct_idx_found = false;
        int x1;
        int x2;
        while (sum_diag(adj_matrix) > 0 && curr_dist_to_consider < num_pairs) {
            int full_idx = dist_idxs[curr_dist_to_consider];
            int i_idx;
            int j_idx;
            get_ij_from_pascal_idx(full_idx, 0, &i_idx, &j_idx);

            put_val_in_array_i(0, i_idx, i_idx, adj_matrix);
            put_val_in_array_i(0, j_idx, j_idx, adj_matrix);

            put_val_in_array_i(1, i_idx, j_idx, adj_matrix);
            put_val_in_array_i(1, j_idx, i_idx, adj_matrix);

            x1 = positions->data[i_idx][0];
            x2 = positions->data[j_idx][0];

            curr_dist_to_consider++;
        }
        printf("Found x1=%i and x2=%i\n", x1, x2);
        printf("product=%li\n", (long)x1 * x2);

    } else {
        printf("Need to do better\n");
        /* What would have happened in that case (still 2 or more component left):
         * 1) modify the find_subgraphs() to pass in previous state (to not redo previous work)
         * 2) while loop to wait until we have 1 big connected component. Save last pascal index
         * put into adjacency matrix while completing graph
         * 3) find the x1 and x2 from this last pascal index.
         */
    }

    // memory management
    free_array_i(positions);
    free_array_i(adj_matrix);
    free(pairwise_distance);
    free(dist_idxs);

    fclose(input_file);
    return EXIT_SUCCESS;
}
