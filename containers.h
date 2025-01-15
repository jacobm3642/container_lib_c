#ifndef CONTAINERS_H
#define CONTAINERS_H
#include <stddef.h>
#include <stdbool.h>

typedef void *General_Allocator(size_t blob_size);
typedef void *Specific_Allocator(size_t blob_size, void *allocator_struct);

typedef void General_Deallocator(void *ptr);
typedef void Specific_Deallocator(void *ptr, void *allocator_struct);

typedef struct Con_Dynamic_Array {
        size_t Block_Size;
        size_t Max_Items;
        size_t Cur_Items;
        size_t sequential_access_count;

        bool use_general_allocator;

        union alloctaor {
                General_Allocator *general_allocator;
                Specific_Allocator *specific_allocator;
        } alloctaor;

        union dealloctaor {
                General_Deallocator *general_deallocator;
                Specific_Deallocator *specific_deallocator;
        } dealloctaor;

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

#endif // !CONTAINERS_H
