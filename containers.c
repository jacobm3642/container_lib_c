#include "containers.h"

#include <stddef.h>
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

void *use_allocator(size_t size, Con_Dynamic_Array *target)
{
        if (target->use_general_allocator) {
                return target->alloctaor.general_allocator(size);
        } else {
                return target->alloctaor.specific_allocator(size, target->allocator_struct);
        }
}

void use_deallocator(void *ptr, Con_Dynamic_Array *target)
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
                void *new_blob = use_allocator(target->Max_Items*2, target);

                memcpy(new_blob, target->blob, target->Block_Size * target->Cur_Items);
                use_deallocator(target->blob, target);
                
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

// sort isnt being inplmented at this point
