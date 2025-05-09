#ifndef CONTAINERS_H
#define CONTAINERS_H
#include <stddef.h>
#include <stdbool.h>
#include <stdint.h>

typedef void *General_Allocator(size_t blob_size);
typedef void *Specific_Allocator(size_t blob_size, void *allocator_struct);

typedef void General_Deallocator(void *ptr);
typedef void Specific_Deallocator(void *ptr, void *allocator_struct);

union Alloctaor {
        General_Allocator *general_allocator;
        Specific_Allocator *specific_allocator;
};

union Dealloctaor {
        General_Deallocator *general_deallocator;
        Specific_Deallocator *specific_deallocator;
};

/*
 * ---- Dynamic array ----
 * */

typedef struct Con_Dynamic_Array {
        size_t Block_Size;
        size_t Max_Items;
        size_t Cur_Items;
        size_t sequential_access_count;

        bool use_general_allocator;
        
        union Alloctaor alloctaor;
        union Dealloctaor dealloctaor;

        void *allocator_struct;

        void *blob;
} Con_Dynamic_Array;



Con_Dynamic_Array init_dynamic_array(size_t Block_size, void *alloc_funtion, void *free_function, void *allocator_struct);
void dynamic_array_insert(void *data, size_t size_of_data, Con_Dynamic_Array *target);
void dynamic_array_remove(size_t index, Con_Dynamic_Array *target);
void reset_sequential_access_dynamic_array(Con_Dynamic_Array *target);
void *sequential_access_dynamic_array(Con_Dynamic_Array *target);
void *random_access_dynamic_array(size_t index, Con_Dynamic_Array *target);
int find_dynamic_array(void *pattern, Con_Dynamic_Array *target); 
void free_dymanic_array(Con_Dynamic_Array *target);

/* 
 * Linked List
 * */

typedef struct Con_Linked_List_Node {
        struct Con_Linked_List_Node *next;
        struct Con_Linked_List_Node *prev;
        void *data;
} Con_Linked_List_Node;

typedef struct Con_Linked_List {
        
        Con_Linked_List_Node *head;
        Con_Linked_List_Node *sequential_access_ptr;
        unsigned int count;
        
        union Alloctaor alloctaor;
        union Dealloctaor dealloctaor;
        
        void *allocator_struct;

        bool use_general_allocator;
        bool vaild; 
} Con_Linked_List;

Con_Linked_List init_linked_list(void *alloc_funtion, void *free_function, void *allocator_struct);
void insert_linked_list(void *data, int position, Con_Linked_List *target);
void remove_linked_list(int position, Con_Linked_List *target);
void *random_access_linked_list(int position, Con_Linked_List *target);
void *sequential_access_linked_list(Con_Linked_List *target);
void reset_sequential_access_linked_list(Con_Linked_List *target);
void free_linked_list(Con_Linked_List *target);

/*
 * Stack 
 * */

typedef struct Con_Stack {
        Con_Linked_List linked_list;
        Con_Linked_List_Node *tail_node;
        bool vaild;
} Con_Stack;

Con_Stack init_stack(void *alloc_funtion, void *free_function, void *allocator_struct);
void push_stack(void *data, Con_Stack *target);
void *pop_stack(Con_Stack *target);

/*
 * Queue 
 * */

typedef enum {linked_list, array} Queue_Type;

typedef struct Con_Queue {
        Queue_Type underlaying_data_type;
        union underlaying_data {
                Con_Linked_List linked_list;
                Con_Dynamic_Array array;
        } underlaying_data;

        Con_Linked_List_Node *end_node;
} Con_Queue;

Con_Queue init_queue(size_t known_size, void *alloc_funtion, void *free_function, void *allocator_struct);
void *peek_queue(Con_Queue *target);
void free_queue(Con_Queue *target);
void push_queue(void *data, Con_Queue *queue);
void *pop_queue(Con_Queue *queue);

/*
 * Hash Table
 * */

#define HASH_EMPTY 0  // Empty slot
#define HASH_DELETED 1  // Deleted slot
#define HASH_OCCUPIED 2  // Occupied slot

typedef struct Con_Hash_Entry {
    void* key;
    size_t key_size;
    void* value;
    uint8_t status; // HASH_EMPTY, HASH_DELETED, or HASH_OCCUPIED
} Con_Hash_Entry;

typedef struct Con_Hash_Table {
    Con_Dynamic_Array entries;  // Array of Con_Hash_Entry
    size_t capacity;           // Total capacity of the hash table
    size_t size;               // Current number of elements
    float load_factor;         // Maximum load factor before resize (typically 0.75)
    bool use_general_allocator;
    union Alloctaor allocator;
    union Dealloctaor deallocator;
    void* allocator_struct;
} Con_Hash_Table;

Con_Hash_Table init_hash_table(size_t initial_capacity, float load_factor, void* alloc_function, void* free_function, void* allocator_struct);
void hash_table_insert(const void* key, size_t key_size, void* value, Con_Hash_Table* target);
void* hash_table_lookup(const void* key, size_t key_size, Con_Hash_Table* target);
bool hash_table_remove(const void* key, size_t key_size, Con_Hash_Table* target);
void hash_table_clear(Con_Hash_Table* target);
void free_hash_table(Con_Hash_Table* target);

size_t hash_table_size(Con_Hash_Table* target);
bool hash_table_contains_key(const void* key, size_t key_size, Con_Hash_Table* target);
bool hash_table_is_empty(Con_Hash_Table* target);
float hash_table_load(Con_Hash_Table* target);

#endif // !CONTAINERS_H
