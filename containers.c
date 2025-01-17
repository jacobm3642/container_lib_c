#include "containers.h"

#include <assert.h>
#include <math.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <strings.h>

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
                        new_dynarray.alloctaor.general_allocator = alloc_funtion;
                        new_dynarray.dealloctaor.specific_deallocator = free_function;
                
                } else {
                        new_dynarray.use_general_allocator = false;
                        new_dynarray.alloctaor.specific_allocator = alloc_funtion;
                        new_dynarray.dealloctaor.specific_deallocator = free_function;
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

void *use_allocator_dy(size_t size, Con_Dynamic_Array *target)
{
        if (target->use_general_allocator) {
                return target->alloctaor.general_allocator(size);
        } else {
                return target->alloctaor.specific_allocator(size, target->allocator_struct);
        }
}

void use_deallocator_dy(void *ptr, Con_Dynamic_Array *target)
{
        if (target->use_general_allocator) {
                target->dealloctaor.general_deallocator(ptr);
        } else {
                target->dealloctaor.specific_deallocator(ptr, target->allocator_struct);
        }
}

void dynamic_array_insert(void *data, size_t size_of_data, Con_Dynamic_Array *target)
{
        if (target->Max_Items == target->Cur_Items) {
                void *new_blob = use_allocator_dy(target->Max_Items*2, target);

                memcpy(new_blob, target->blob, target->Block_Size * target->Cur_Items);
                use_deallocator_dy(target->blob, target);
                
                target->Max_Items = target->Max_Items*2;
                target->blob = new_blob;
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
                return (void *)0;
        }
        target->sequential_access_count++;
        return out_ptr;
}

void *random_access_dynamic_array(size_t index, Con_Dynamic_Array *target)
{
        if (index <= target->Cur_Items) {
                return target->blob + (index * target->Block_Size);
        }
        return (void *)0;
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

void *use_allocator_ll(size_t size, Con_Linked_List *target)
{
        if (target->use_general_allocator) {
                return target->alloctaor.general_allocator(size);
        } else {
                return target->alloctaor.specific_allocator(size, target->allocator_struct);
        }
}

void use_deallocator_ll(void *ptr, Con_Linked_List *target)
{
        if (target->use_general_allocator) {
                target->dealloctaor.general_deallocator(ptr);
        } else {
                target->dealloctaor.specific_deallocator(ptr, target->allocator_struct);
        }
}

Con_Linked_List init_linked_list(void *alloc_funtion, void *free_function, void *allocator_struct)
{
        Con_Linked_List new_linked_list = {.head = (void *)0, .vaild = true, .sequential_access_ptr = (void *)0, .use_general_allocator = true, .count = 0};
        if (alloc_funtion == NULL && free_function == NULL && allocator_struct == NULL) {
                new_linked_list.alloctaor.general_allocator = malloc;
                new_linked_list.dealloctaor.general_deallocator = free;

        } else if (alloc_funtion != NULL && free_function != NULL && allocator_struct == NULL){
                if (allocator_struct == NULL) {
                        new_linked_list.alloctaor.general_allocator = alloc_funtion;
                        new_linked_list.dealloctaor.specific_deallocator = free_function;
                
                } else {
                        new_linked_list.use_general_allocator = false;
                        new_linked_list.alloctaor.specific_allocator = alloc_funtion;
                        new_linked_list.dealloctaor.specific_deallocator = free_function;
                        new_linked_list.allocator_struct = allocator_struct;
                }
        } else {
                Con_Linked_List failed_output = {.vaild = false};
                return failed_output;
        }

        return new_linked_list;
}

void recersive_insert_linked_list(Con_Linked_List_Node *node, Con_Linked_List_Node *cur_ptr, int position, Con_Linked_List *target)
{
        if (cur_ptr == (void *)0 || position < 0) {
                return;
        }
        if (position == 0 && cur_ptr != (void *)0) {
                if (cur_ptr->next != (void *)0) {
                        cur_ptr->next->prev = node;
                }
                node->next = cur_ptr->next;
                node->prev = cur_ptr;
                cur_ptr->next = node;
                return;
        }
        recersive_insert_linked_list(node, cur_ptr->next, position-1, target);
        return;
}

void insert_linked_list(void *data, int position, Con_Linked_List *target)
{
        if (position == -1) position = target->count;
        
        Con_Linked_List_Node *new_node = (Con_Linked_List_Node *)use_allocator_ll(sizeof(Con_Linked_List_Node), target);
        new_node->data = data;
        new_node->next = (void *)0;
        new_node->prev = (void *)0;

        if (target->head == (void *)0 && position == 0) { 
                target->head = new_node;
                target->sequential_access_ptr = target->head;
                return;
        } else if (position == 0) {
                new_node->next = target->head;
                target->head->prev = new_node;
                target->head = new_node;
                return;
        }

        if (position != 0) position--;
        recersive_insert_linked_list(new_node, target->head, position, target);
        target->count++;
}

void recersive_remove_linked_list(int position, Con_Linked_List_Node *cur_ptr, Con_Linked_List *target)
{
        if (position < 0 || target == (void *)0 || cur_ptr == (void *)0) {
                return;
        }
        if (position == 0) {
                if (cur_ptr->prev != (void *)0) {
                        cur_ptr->prev->next = cur_ptr->next;
                }
                if (cur_ptr->next != (void *)0) { 
                        cur_ptr->next->prev = cur_ptr->prev;
                }
                use_deallocator_ll(cur_ptr, target);
                return;
        }
        recersive_remove_linked_list(position-1, cur_ptr->next, target);
}

void remove_linked_list(int position, Con_Linked_List *target)
{
        recersive_remove_linked_list(position, target->head, target);
}

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
        return recersive_random_access_linked_list(position, target->head, target);
}

void *sequential_access_linked_list(Con_Linked_List *target)
{
        void *out = (void *)0;
        if (target->sequential_access_ptr == (void *)0) {
                return out;
        }
        out = target->sequential_access_ptr->data;
        target->sequential_access_ptr = target->sequential_access_ptr->next;
        return out;
}

void reset_sequential_access_linked_list(Con_Linked_List *target) 
{
        target->sequential_access_ptr = target->head;
}

void free_linked_list(Con_Linked_List *target)
{
        assert(1==3);
}
