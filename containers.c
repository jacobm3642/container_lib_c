#include "containers.h"

#include <stddef.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

/*
 * ---- Dynamic array ---
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
        Con_Dynamic_Array new_dynarray = {.Block_Size = Block_size, .use_general_allocator = true, .Cur_Items = 0, .Max_Items = 1};
        
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
