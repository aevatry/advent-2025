#ifndef MY_LIST_H
#define MY_LIST_H

typedef struct IntList {
    int max_n;
    int idx_next_elem;
    int *data;
} IntList;

IntList *create_list_i(int max_size);
void free_list_i(IntList *list);
void fill_list_i(int val, IntList *list);
void push_val_in_list_i(int val, IntList *list);
void print_list(IntList *list);
int is_list_i_strictly_equal(IntList *list1, IntList *list2);


// List of lists
typedef struct IntListOL {
    int num_sublists;
    int allocated_sublists;
    IntList **all_lists;
} IntListOL;

IntListOL *create_listol_i();
void add_sub_list_to_listol(int sub_list_size, IntListOL *list_of_lists);
void free_listol_i(IntListOL *list_of_lists);
void push_val_in_listol_i(int val, int list_idx, IntListOL *list_of_lists);
IntList *get_sub_list(int list_idx, IntListOL *list_of_lists);

#endif
