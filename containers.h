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

#endif // !CONTAINERS_H
