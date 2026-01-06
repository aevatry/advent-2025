#include "list.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

const int default_list_size = 32;
const int min_num_list = 4;

IntList *create_list_i(int max_size) {
    IntList *list = malloc(sizeof(IntList));
    if (max_size > 0) {
        list->data = calloc(max_size, sizeof(*list->data));
    } else {
        list->data = calloc(default_list_size, sizeof(*list->data));
        max_size = default_list_size;
    }

    assert(list->data != NULL);

    list->max_n = max_size;
    list->idx_next_elem = 0;
    return list;
};

void free_list_i(IntList *list) {
    free(list->data);
    free(list);
};

void fill_list_i(int val, IntList *list) {
    for (int i = 0; i < list->max_n; i++) {
        list->data[i] = val;
    }
};

void push_val_in_list_i(int val, IntList *list) {

    // bound checking
    assert(list->idx_next_elem <= list->max_n);

    if (list->idx_next_elem == list->max_n) {
        // realloc memory
        int *new_ptr = realloc(list->data, sizeof(*list->data) * 2 * list->max_n);

        // error handling
        if (new_ptr == NULL) {
            printf("Exiting. Could not realloc list\n");
            exit(0);
        }

        list->data = new_ptr;
        list->max_n = 2 * list->max_n;
    }

    list->data[list->idx_next_elem] = val;
    list->idx_next_elem++;
};

void print_list(IntList *list) {
    printf("(");
    for (int i = 0; i < list->idx_next_elem - 1; i++) {
        printf("%i, ", list->data[i]);
    }
    printf("%i)\n", list->data[list->idx_next_elem - 1]);
};

int is_list_i_strictly_equal(IntList *list1, IntList *list2) {
    // 0 is false and anything else is true

    if (list1->idx_next_elem != list2->idx_next_elem) {
        return 0;
    }

    for (int i = 0; i < list1->idx_next_elem; i++) {
        if (list1->data[i] != list2->data[i]) {
            return 0;
        }
    }

    return 1;
};

// List of lists
IntListOL *create_listol_i() {
    IntListOL *nested_list = malloc(sizeof(*nested_list));
    nested_list->num_sublists = 0;
    nested_list->allocated_sublists = 0;
    return nested_list;
};
void add_sub_list_to_listol(int sub_list_size, IntListOL *list_of_lists) {
    // base case, 0 sublist, allocate space for a few list pointers
    if (list_of_lists->allocated_sublists == 0) {
        list_of_lists->all_lists = calloc(min_num_list, sizeof(*list_of_lists->all_lists));
        list_of_lists->allocated_sublists = min_num_list;
    }

    // assert the list of list has not been modified weirdly
    assert(list_of_lists->num_sublists <= list_of_lists->allocated_sublists);

    // reallocation if we need to grow the number of lists
    if (list_of_lists->num_sublists == list_of_lists->allocated_sublists) {
        int new_num_max = 2 * list_of_lists->allocated_sublists;
        IntList **ptr =
            realloc(list_of_lists->all_lists, new_num_max * sizeof(*list_of_lists->all_lists));
        if (ptr == NULL) {
            printf("Realloc of the list of lists failed\n");
            exit(EXIT_FAILURE);
        }
        list_of_lists->all_lists = ptr;
        list_of_lists->allocated_sublists = new_num_max;
    }

    int index_next_sublist = list_of_lists->num_sublists;
    list_of_lists->all_lists[index_next_sublist] = create_list_i(sub_list_size);

    list_of_lists->num_sublists += 1;
};
void free_listol_i(IntListOL *list_of_lists) {
    for (int i = 0; i < list_of_lists->num_sublists; i++) {
        free_list_i(list_of_lists->all_lists[i]);
    }
    free(list_of_lists);
};
void push_val_in_listol_i(int val, int list_idx, IntListOL *list_of_lists) {
    assert(list_idx < list_of_lists->num_sublists);
    IntList *list = list_of_lists->all_lists[list_idx];
    push_val_in_list_i(val, list);
};
IntList *get_sub_list(int list_idx, IntListOL *list_of_lists) {
    assert(list_idx < list_of_lists->num_sublists);
    IntList *list = list_of_lists->all_lists[list_idx];
    return list;
};
