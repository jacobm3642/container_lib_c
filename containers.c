#include "containers.h"

#include <assert.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <strings.h>
#include <stdint.h>

/*
 * ---- Dynamic array ----
 * */

/*
 * init function
 * 
 * args:
 *      size_t block_size
 *      
 *      void *allocator_function (if NULL is passed malloc will be used)
 *      void *free_function (if NULL is passed free will be used)
 *      
 *      void *alloctor_struct (if NULL is passed the alloctor and free function
 *      are assumed to have the structure of:
 *              void *General_Allocator(size_t blob_size); and
 *              void General_Deallocator(void *ptr);
 *      else:
 *              void *Specific_Allocator(size_t blob_size, void *allocator_struct);
 *              void Specific_Deallocator(void *ptr, void *allocator_struct);
 *      )
 * */
Con_Dynamic_Array init_dynamic_array(size_t Block_size, void *alloc_funtion, void *free_function, void *allocator_struct)
{
        Con_Dynamic_Array new_dynarray = {.Block_Size = Block_size,.sequential_access_count = 0, .use_general_allocator = true, .Cur_Items = 0, .Max_Items = 1};

        if (alloc_funtion == NULL && free_function == NULL && allocator_struct == NULL) {
                new_dynarray.alloctaor.general_allocator = malloc;
                new_dynarray.dealloctaor.general_deallocator = free;

        } else if (alloc_funtion != NULL && free_function != NULL && allocator_struct == NULL){
                if (allocator_struct == NULL) {
                        General_Allocator *alloc_funtion_fn = alloc_funtion;
                        General_Deallocator *free_function_fn = free_function;

                        new_dynarray.alloctaor.general_allocator = alloc_funtion_fn;
                        new_dynarray.dealloctaor.general_deallocator = free_function_fn;

                } else {
                        Specific_Allocator *alloc_funtion_fn = alloc_funtion;
                        Specific_Deallocator *free_function_fn = free_function;
                        new_dynarray.use_general_allocator = false;
                        new_dynarray.alloctaor.specific_allocator = alloc_funtion_fn;
                        new_dynarray.dealloctaor.specific_deallocator = free_function_fn;
                        new_dynarray.allocator_struct = allocator_struct;
                }
        } else {
                Con_Dynamic_Array failed_output = {0};
                return failed_output;
        }

        if (new_dynarray.use_general_allocator) {
                new_dynarray.blob = new_dynarray.alloctaor.general_allocator(new_dynarray.Block_Size);
        } else {
                new_dynarray.blob = new_dynarray.alloctaor.specific_allocator(new_dynarray.Block_Size, new_dynarray.allocator_struct);
        }

        return new_dynarray;
}

// private (should only be used within this translation unit)
void *use_allocator_dy(size_t size, Con_Dynamic_Array *target)
{
        if (target->use_general_allocator) {
                return target->alloctaor.general_allocator(size);
        } else {
                return target->alloctaor.specific_allocator(size, target->allocator_struct);
        }
}

// private (should only be used within this translation unit)
void use_deallocator_dy(void *ptr, Con_Dynamic_Array *target)
{
        if (target->use_general_allocator) {
                target->dealloctaor.general_deallocator(ptr);
        } else {
                target->dealloctaor.specific_deallocator(ptr, target->allocator_struct);
        }
}

void resize_array(Con_Dynamic_Array *target, size_t new_size)
{
        if (new_size <= target->Max_Items){
                return;
        }      

        void *new_blob = use_allocator_dy(new_size, target);

        memcpy(new_blob, target->blob, target->Block_Size * target->Cur_Items);
        use_deallocator_dy(target->blob, target);

        target->Max_Items = new_size;
        target->blob = new_blob;
}

void dynamic_array_insert(void *data, size_t size_of_data, Con_Dynamic_Array *target)
{
        if (target->Max_Items == target->Cur_Items) {
                resize_array(target, target->Max_Items*2);
        }

        if (size_of_data == target->Block_Size) {
                memcpy(target->blob + (target->Block_Size * target->Cur_Items), data, size_of_data);
                target->Cur_Items++;
        }
}

void dynamic_array_remove(size_t index, Con_Dynamic_Array *target)
{
        if (index <= target->Cur_Items) {
                memmove(target->blob + (target->Block_Size * index), 
                        target->blob + (target->Block_Size * (index + 1)), 
                        (target->Cur_Items - index - 1) * target->Block_Size);

                target->Cur_Items--;
        }
}

void reset_sequential_access_dynamic_array(Con_Dynamic_Array *target)
{
        target->sequential_access_count = 0;
}

void *sequential_access_dynamic_array(Con_Dynamic_Array *target)
{
        void *out_ptr = target->blob + ((target->sequential_access_count) * target->Block_Size);
        if(target->sequential_access_count > target->Max_Items) {
                reset_sequential_access_dynamic_array(target);
                return (void *)0;
        }
        target->sequential_access_count++;
        return out_ptr;
}

void random_insert_dynamic_array(size_t pos, void *data, size_t size_of_data, Con_Dynamic_Array *target)
{
        if (pos > target->Cur_Items) {
                return;
        }

        if (target->Cur_Items == target->Max_Items) {
                void *new_blob = use_allocator_dy(target->Max_Items * 2, target);
                memcpy(new_blob, target->blob, target->Block_Size * target->Cur_Items);
        }
        memmove(target->blob + (target->Block_Size * (pos + 1)), target->blob + (target->Block_Size * pos), (target->Cur_Items - pos) * target->Block_Size);
        memmove(target->blob + (target->Block_Size * pos), data, size_of_data);
        target->Cur_Items++;
}

void *random_access_dynamic_array(size_t index, Con_Dynamic_Array *target)
{
        if (index <= target->Cur_Items) {
                return target->blob + (index * target->Block_Size);
        }
        return (void *)0;
}

void random_write_dynamic_array(void *value, size_t index, Con_Dynamic_Array * target)
{
        if (index >= target->Max_Items) {
                return;
        }
        void *target_loc = (void *)(target->blob + (index * target->Block_Size));
        memcpy(target_loc, value, target->Block_Size);
}

int find_dynamic_array(void *pattern, Con_Dynamic_Array *target) 
{
        int index = -1;
        for (void *item = sequential_access_dynamic_array(target); item != (void *)0 ; item = sequential_access_dynamic_array(target)) {
                if (memcmp(item, pattern, target->Block_Size)) {
                        index = target->sequential_access_count;
                        break;
                }
        }
        reset_sequential_access_dynamic_array(target);
        return index;
}

void free_dymanic_array(Con_Dynamic_Array *target)
{
        use_deallocator_dy(target->blob, target);
}

// sort isnt being inplmented at this point


/* 
 * Linked List
 * */

// private (should only be used within this translation unit)
void *use_allocator_ll(size_t size, Con_Linked_List *target)
{
        if (target->use_general_allocator) {
                return target->alloctaor.general_allocator(size);
        } else {
                return target->alloctaor.specific_allocator(size, target->allocator_struct);
        }
}

// private (should only be used within this translation unit)
void use_deallocator_ll(void *ptr, Con_Linked_List *target)
{
        if (target->use_general_allocator) {
                target->dealloctaor.general_deallocator(ptr);
        } else {
                target->dealloctaor.specific_deallocator(ptr, target->allocator_struct);
        }
}

// private (should only be used within this translation unit)
void remove_node_ll(Con_Linked_List_Node *cur_ptr, Con_Linked_List * target)
{
        if (cur_ptr == (void *)0) {
                return;
        }

        if (cur_ptr == target->head) {
                target->head = cur_ptr->next;
        }
        if (cur_ptr->prev != (void *)0) {
                cur_ptr->prev->next = cur_ptr->next;
        } else {
                target->head = cur_ptr->next;
        }

        if (cur_ptr->next != (void *)0) { 
                cur_ptr->next->prev = cur_ptr->prev;
        }

        use_deallocator_ll(cur_ptr, target);
        target->count--;
}

// private (should only be used within this translation unit)
void add_node_ll(Con_Linked_List_Node *node, Con_Linked_List_Node *cur_ptr)
{
        if (node == (void *)0 || cur_ptr == (void *)0) {
                return;
        }

        node->next = cur_ptr->next;
        node->prev = cur_ptr;

        if (cur_ptr->next != (void *)0) {
                cur_ptr->next->prev = node;
        }

        cur_ptr->next = node;
}

// private (should only be used within this translation unit)
Con_Linked_List_Node *make_new_node(void *data, Con_Linked_List *target)
{
        Con_Linked_List_Node *new_node = (Con_Linked_List_Node *)use_allocator_ll(sizeof(Con_Linked_List_Node), target);
        new_node->data = data;
        new_node->next = (void *)0;
        new_node->prev = (void *)0;
        return new_node;
}

Con_Linked_List init_linked_list(void *alloc_funtion, void *free_function, void *allocator_struct)
{
        Con_Linked_List new_linked_list = {.head = (void *)0, .vaild = true, .sequential_access_ptr = (void *)0, .use_general_allocator = true, .count = 0};
        if (alloc_funtion == NULL && free_function == NULL && allocator_struct == NULL) {
                new_linked_list.alloctaor.general_allocator = malloc;
                new_linked_list.dealloctaor.general_deallocator = free;

        } else if (alloc_funtion != NULL && free_function != NULL && allocator_struct == NULL){
                if (allocator_struct == NULL) {
                        General_Allocator *alloc_funtion_fn = alloc_funtion;
                        General_Deallocator *free_function_fn = free_function;

                        new_linked_list.alloctaor.general_allocator = alloc_funtion_fn;
                        new_linked_list.dealloctaor.general_deallocator = free_function_fn;

                } else {
                        Specific_Allocator *alloc_funtion_fn = alloc_funtion;
                        Specific_Deallocator *free_function_fn = free_function;

                        new_linked_list.use_general_allocator = false;
                        new_linked_list.alloctaor.specific_allocator = alloc_funtion_fn;
                        new_linked_list.dealloctaor.specific_deallocator = free_function_fn;
                        new_linked_list.allocator_struct = allocator_struct;
                }
        } else {
                Con_Linked_List failed_output = {.vaild = false};
                return failed_output;
        }

        return new_linked_list;
}

// private (should only be used within this translation unit)
void recersive_insert_linked_list(Con_Linked_List_Node *node, Con_Linked_List_Node *cur_ptr, int position, Con_Linked_List *target)
{
        if (position < 0) {
                return;
        }
        if (cur_ptr->next == NULL && position == 1) {
                add_node_ll(node, cur_ptr);
                target->count++;
                return;
        }

        if (position == 0) {
                add_node_ll(node, cur_ptr);
                target->count++;
                return;
        }

        recersive_insert_linked_list(node, cur_ptr->next, position-1, target);
}

void insert_linked_list(void *data, int position, Con_Linked_List *target)
{
        if (position == -1) position = target->count;
        if (position < 0) return;

        Con_Linked_List_Node *new_node = make_new_node(data, target);
        if (new_node == (void *)0) return;

        if (target->head == (void *)0) { 
                target->head = new_node;
                target->sequential_access_ptr = target->head;
                target->count = 1;
                return;
        }

        if (position == 0) {
                new_node->next = target->head;
                target->head->prev = new_node;
                target->head = new_node;
                target->count++;
                return;
        }

        if (position > target->count) {
                position = target->count;
        }

        recersive_insert_linked_list(new_node, target->head, position - 1, target);
        target->count++;
}

// private (should only be used within this translation unit)
void recersive_remove_linked_list(int position, Con_Linked_List_Node *cur_ptr, Con_Linked_List *target)
{
        if (position < 0 || target == (void *)0 || cur_ptr == (void *)0) {
                return;
        }

        if (position == 0) {
                remove_node_ll(cur_ptr, target);
                return;
        }

        recersive_remove_linked_list(position-1, cur_ptr->next, target);
}

void remove_linked_list(int position, Con_Linked_List *target)
{
        if (position == -1) position = target->count - 1;
        if (position < 0 || position >= target->count) return;

        if (position == 0) {
                Con_Linked_List_Node *old_head = target->head;
                target->head = target->head->next;
                if (target->head != (void *)0) {
                        target->head->prev = (void *)0;
                }
                use_deallocator_ll(old_head, target);
                target->count--;
                return;
        }

        if (target->count == 0) return;
        recersive_remove_linked_list(position, target->head, target);
        target->count--;
}

// private (should only be used within this translation unit)
void *recersive_random_access_linked_list(int position, Con_Linked_List_Node *cur_ptr, Con_Linked_List *target)
{
        if (position < 0 || target == (void *)0 || cur_ptr == (void *)0) {
                return (void *)0;
        }
        if (position == 0) {
                return cur_ptr->data;
        }
        return recersive_random_access_linked_list(position-1, cur_ptr->next, target);

}
void *random_access_linked_list(int position, Con_Linked_List *target)
{
        if (position < 0 || target == (void *)0 || target->head == (void *)0) {
                return (void *)0;
        }
        if (position >= target->count) {
                return (void *)0;
        }
        return recersive_random_access_linked_list(position, target->head, target);
}

void reset_sequential_access_linked_list(Con_Linked_List *target) 
{
        target->sequential_access_ptr = target->head;
}

void *sequential_access_linked_list(Con_Linked_List *target)
{
        void *out = (void *)0;
        if (target->sequential_access_ptr == (void *)0) {
                reset_sequential_access_linked_list(target);
                return out;
        }
        out = target->sequential_access_ptr->data;
        target->sequential_access_ptr = target->sequential_access_ptr->next;
        return out;
}

// private (should only be used within this translation unit)
void recersive_free_linked_list(Con_Linked_List_Node *node, Con_Linked_List *target)
{
        if (node == (void *)0) {
                return;
        }
        recersive_free_linked_list(node->next, target);
        use_deallocator_ll(node, target);
}

void free_linked_list(Con_Linked_List *target)
{
        recersive_free_linked_list(target->head, target);
}

/*
 * Stack 
 * */

Con_Stack init_stack(void *alloc_funtion, void *free_function, void *allocator_struct)
{
        Con_Stack new_stack;
        new_stack.linked_list = init_linked_list(alloc_funtion, free_function, allocator_struct);
        new_stack.tail_node = new_stack.linked_list.head;
        new_stack.vaild = new_stack.linked_list.vaild;
        return new_stack;
}

void push_stack(void *data, Con_Stack *target)
{
        if (target->linked_list.head == (void *)0){
                insert_linked_list(data, 0, &target->linked_list);
                target->tail_node = target->linked_list.head;        
                return;
        }
        Con_Linked_List_Node *new_node = make_new_node(data, &target->linked_list);
        add_node_ll(new_node, target->tail_node);
        target->tail_node = new_node;
}

void *pop_stack(Con_Stack *target)
{
        if (target->tail_node == (void *)0) {
                return (void *)0;
        }
        void *out_data = target->tail_node->data;
        Con_Linked_List_Node *old_tail = target->tail_node;
        target->tail_node = target->tail_node->prev;
        remove_node_ll(old_tail, &target->linked_list);
        return out_data;
}

/*
 * queue
 * */

Con_Queue init_queue(void *alloc_funtion, void *free_function, void *allocator_struct)
{
        // array type not implemeted rn 
        Con_Queue new_queue = {.end_node = NULL};
        new_queue.underlaying_data_type = linked_list;
        new_queue.underlaying_data.linked_list = init_linked_list(alloc_funtion, free_function, allocator_struct);
        new_queue.end_node = new_queue.underlaying_data.linked_list.head;
        return new_queue;
}

void push_queue(void *data, Con_Queue *queue)
{
        if (queue->underlaying_data.linked_list.head == (void *)0) {
                insert_linked_list(data, 0, &queue->underlaying_data.linked_list);
                queue->end_node = queue->underlaying_data.linked_list.head;
                return;
        }

        Con_Linked_List_Node *new_node = make_new_node(data, &queue->underlaying_data.linked_list);
        add_node_ll(new_node, queue->end_node);
        queue->end_node = new_node;
        queue->underlaying_data.linked_list.count++;
}

void *pop_queue(Con_Queue *queue)
{
        void *out = random_access_linked_list(0, &queue->underlaying_data.linked_list);
        remove_linked_list(0, &queue->underlaying_data.linked_list);
        if (queue->underlaying_data.linked_list.head == (void *)0) {
                queue->end_node = (void *)0;
        }
        return out;
}

void *peek_queue(Con_Queue *target)
{
        if (target->underlaying_data_type == array) {
                if (target->underlaying_data.array.Cur_Items == 0) {
                        return (void *)0;
                }
                return random_access_dynamic_array(target->underlaying_data.array.Cur_Items - 1, &target->underlaying_data.array);
        } else {
                if (target->underlaying_data.linked_list.head == (void *)0) {
                        return (void *)0;
                }
                return target->underlaying_data.linked_list.head->data;
        }
}

void free_queue(Con_Queue *target)
{
        if (target->underlaying_data_type == array) {
                free_dymanic_array(&target->underlaying_data.array);
        } else {
                free_linked_list(&target->underlaying_data.linked_list);
        }
}

/*
 * hash table
 * */

// private (should only be used within this translation unit)
uint32_t jenkins_one_at_a_time_hash(const uint8_t* key, size_t length) {
        size_t i = 0;
        uint32_t hash = 0;
        while (i != length) {
                hash += key[i++];
                hash += hash << 10;
                hash ^= hash >> 6;
        }
        hash += hash << 3;
        hash ^= hash >> 11;
        hash += hash << 15;
        return hash;
}

// Private helper to create a new hash entry
static Con_Hash_Entry create_hash_entry(const void* key, size_t key_size, void* value) {
        Con_Hash_Entry entry;

        entry.key = malloc(key_size);
        if (entry.key != NULL) {
                memcpy(entry.key, key, key_size);
                entry.key_size = key_size;
                entry.value = value;
                entry.status = HASH_OCCUPIED;
        } else {
                entry.status = HASH_EMPTY;
                entry.key = NULL;
                entry.key_size = 0;
                entry.value = NULL;
        }

        return entry;
}

static Con_Hash_Entry create_empty_entry() {
        Con_Hash_Entry entry;
        entry.key = NULL;
        entry.key_size = 0;
        entry.value = NULL;
        entry.status = HASH_EMPTY;
        return entry;
}

static bool hash_table_resize(Con_Hash_Table* target, size_t new_capacity) {
        Con_Dynamic_Array old_entries = target->entries;
        size_t old_capacity = target->capacity;

        Con_Dynamic_Array new_entries = init_dynamic_array(sizeof(Con_Hash_Entry), 
                                                           target->use_general_allocator ? target->allocator.general_allocator : NULL,
                                                           target->use_general_allocator ? target->deallocator.general_deallocator : NULL,
                                                           target->use_general_allocator ? NULL : target->allocator_struct);

        for (size_t i = 0; i < new_capacity; i++) {
                Con_Hash_Entry empty = create_empty_entry();
                dynamic_array_insert(&empty, sizeof(Con_Hash_Entry), &new_entries);
        }

        target->entries = new_entries;
        target->capacity = new_capacity;
        target->size = 0;  

        for (size_t i = 0; i < old_capacity; i++) {
                Con_Hash_Entry* entry = random_access_dynamic_array(i, &old_entries);
                if (entry != NULL && entry->status == HASH_OCCUPIED) {
                        hash_table_insert(entry->key, entry->key_size, entry->value, target);

                        free(entry->key);
                }
        }

        free_dymanic_array(&old_entries);

        return true;
}

Con_Hash_Table init_hash_table(size_t initial_capacity, float load_factor, void* alloc_function, void* free_function, void* allocator_struct) {
        Con_Hash_Table table = {0};

        if (initial_capacity == 0) {
                initial_capacity = 16; 
        }

        if (load_factor <= 0.0f || load_factor >= 1.0f) {
                load_factor = 0.75f;   
        }

        table.capacity = initial_capacity;
        table.size = 0;
        table.load_factor = load_factor;
        table.use_general_allocator = true;

        if (alloc_function == NULL && free_function == NULL && allocator_struct == NULL) {
                table.allocator.general_allocator = malloc;
                table.deallocator.general_deallocator = free;
        } else if (alloc_function != NULL && free_function != NULL) {
                if (allocator_struct == NULL) {
                        table.allocator.general_allocator = (General_Allocator*)alloc_function;
                        table.deallocator.general_deallocator = (General_Deallocator*)free_function;
                } else {
                        table.use_general_allocator = false;
                        table.allocator.specific_allocator = (Specific_Allocator*)alloc_function;
                        table.deallocator.specific_deallocator = (Specific_Deallocator*)free_function;
                        table.allocator_struct = allocator_struct;
                }
        } else {
                return table;
        }

        table.entries = init_dynamic_array(sizeof(Con_Hash_Entry), 
                                           table.use_general_allocator ? table.allocator.general_allocator : NULL,
                                           table.use_general_allocator ? table.deallocator.general_deallocator : NULL,
                                           table.use_general_allocator ? NULL : table.allocator_struct);

        for (size_t i = 0; i < initial_capacity; i++) {
                Con_Hash_Entry empty = create_empty_entry();
                dynamic_array_insert(&empty, sizeof(Con_Hash_Entry), &table.entries);
        }

        return table;
}

void hash_table_insert(const void* key, size_t key_size, void* value, Con_Hash_Table* target) {
        if ((float)(target->size + 1) / target->capacity > target->load_factor) {
                hash_table_resize(target, target->capacity * 2);
        }

        uint32_t hash = jenkins_one_at_a_time_hash((const uint8_t*)key, key_size) % target->capacity;
        size_t first_deleted = target->capacity; 
        size_t probe_count = 0;

        while (probe_count < target->capacity) {
                Con_Hash_Entry* entry = random_access_dynamic_array(hash, &target->entries);

                if (entry == NULL) {
                        return;
                }

                if (entry->status == HASH_EMPTY) {
                        Con_Hash_Entry new_entry = create_hash_entry(key, key_size, value);
                        memcpy(entry, &new_entry, sizeof(Con_Hash_Entry));
                        target->size++;
                        return;
                }

                if (entry->status == HASH_DELETED && first_deleted == target->capacity) {
                        first_deleted = hash;
                }

                if (entry->status == HASH_OCCUPIED && 
                        entry->key_size == key_size && 
                        memcmp(entry->key, key, key_size) == 0) {
                        entry->value = value;
                        return;
                }

                hash = (hash + 1) % target->capacity;
                probe_count++;
        }

        if (first_deleted < target->capacity) {
                Con_Hash_Entry* entry = random_access_dynamic_array(first_deleted, &target->entries);
                if (entry->key != NULL) {
                        free(entry->key);
                }

                Con_Hash_Entry new_entry = create_hash_entry(key, key_size, value);
                memcpy(entry, &new_entry, sizeof(Con_Hash_Entry));
                target->size++;
        }
}

void* hash_table_lookup(const void* key, size_t key_size, Con_Hash_Table* target) {
        if (target->size == 0) {
                return NULL;
        }

        uint32_t hash = jenkins_one_at_a_time_hash((const uint8_t*)key, key_size) % target->capacity;
        size_t probe_count = 0;

        while (probe_count < target->capacity) {
                Con_Hash_Entry* entry = random_access_dynamic_array(hash, &target->entries);

                if (entry == NULL) {
                        return NULL;
                }

                if (entry->status == HASH_EMPTY) {
                        return NULL;
                }

                if (entry->status == HASH_OCCUPIED && 
                        entry->key_size == key_size && 
                        memcmp(entry->key, key, key_size) == 0) {
                        return entry->value;
                }

                hash = (hash + 1) % target->capacity;
                probe_count++;
        }

        return NULL; 
}

bool hash_table_remove(const void* key, size_t key_size, Con_Hash_Table* target) {
        if (target->size == 0) {
                return false;
        }

        uint32_t hash = jenkins_one_at_a_time_hash((const uint8_t*)key, key_size) % target->capacity;
        size_t probe_count = 0;

        while (probe_count < target->capacity) {
                Con_Hash_Entry* entry = random_access_dynamic_array(hash, &target->entries);

                if (entry == NULL) {
                        return false;
                }

                if (entry->status == HASH_EMPTY) {
                        return false;
                }

                if (entry->status == HASH_OCCUPIED && 
                        entry->key_size == key_size && 
                        memcmp(entry->key, key, key_size) == 0) {
                        free(entry->key);
                        entry->key = NULL;
                        entry->key_size = 0;
                        entry->value = NULL;
                        entry->status = HASH_DELETED;
                        target->size--;
                        return true;
                }

                hash = (hash + 1) % target->capacity;
                probe_count++;
        }

        return false;
}

void hash_table_clear(Con_Hash_Table* target) {
        for (size_t i = 0; i < target->capacity; i++) {
                Con_Hash_Entry* entry = random_access_dynamic_array(i, &target->entries);
                if (entry != NULL && entry->status == HASH_OCCUPIED) {
                        free(entry->key);
                        entry->key = NULL;
                        entry->key_size = 0;
                        entry->value = NULL;
                        entry->status = HASH_EMPTY;
                }
        }

        target->size = 0;
}

void free_hash_table(Con_Hash_Table* target) {
        hash_table_clear(target);

        free_dymanic_array(&target->entries);

        target->capacity = 0;
}

size_t hash_table_size(Con_Hash_Table* target) {
        return target->size;
}

bool hash_table_contains_key(const void* key, size_t key_size, Con_Hash_Table* target) {
        return hash_table_lookup(key, key_size, target) != NULL;
}

bool hash_table_is_empty(Con_Hash_Table* target) {
        return target->size == 0;
}

float hash_table_load(Con_Hash_Table* target) {
        return (float)target->size / target->capacity;
}

typedef struct Con_Hash_Iterator {
        Con_Hash_Table* table;
        size_t current_index;
        bool has_next;
} Con_Hash_Iterator;

Con_Hash_Iterator hash_table_iterator(Con_Hash_Table* target) {
        Con_Hash_Iterator iterator;
        iterator.table = target;
        iterator.current_index = 0;
        iterator.has_next = (target->size > 0);

        if (iterator.has_next) {
                while (iterator.current_index < target->capacity) {
                        Con_Hash_Entry* entry = random_access_dynamic_array(iterator.current_index, &target->entries);
                        if (entry != NULL && entry->status == HASH_OCCUPIED) {
                                break;
                        }
                        iterator.current_index++;
                }

                if (iterator.current_index >= target->capacity) {
                        iterator.has_next = false;
                }
        }

        return iterator;
}

bool hash_iterator_has_next(Con_Hash_Iterator* iterator) {
        return iterator->has_next;
}

Con_Hash_Entry* hash_iterator_next(Con_Hash_Iterator* iterator) {
        if (!iterator->has_next) {
                return NULL;
        }

        Con_Hash_Entry* entry = random_access_dynamic_array(iterator->current_index, &iterator->table->entries);

        iterator->current_index++;
        iterator->has_next = false;

        while (iterator->current_index < iterator->table->capacity) {
                Con_Hash_Entry* next_entry = random_access_dynamic_array(iterator->current_index, &iterator->table->entries);
                if (next_entry != NULL && next_entry->status == HASH_OCCUPIED) {
                        iterator->has_next = true;
                        break;
                }
                iterator->current_index++;
        }

        return entry;
}
