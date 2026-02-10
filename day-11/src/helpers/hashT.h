#ifndef MY_HASH_TABLE_H
#define MY_HASH_TABLE_H

// Linked List

typedef struct LinkedListNode LinkedListNode;
struct LinkedListNode{
    LinkedListNode* next;
    void *data;
    int data_size;
};

typedef struct LinkedList{
    LinkedListNode * first_node;
    int count;
} LinkedList;

LinkedList *new_linked_list(int node_size, void*data);
void add_node_to_start(int node_size, void*data, LinkedListNode *node, LinkedList *list);
LinkedListNode* find_node(LinkedList *list, void*data_to_find, int(*compare)(const void*, const void*));
int free_linked_list(LinkedList *list, void (*free_data)(const void*));

// Hash table implementation

typedef struct HashTable{
    int num_keys;

} HashTable;



#endif
