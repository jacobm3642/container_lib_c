#include <check.h>
#include <stdio.h>
#include <stdlib.h>

#include "containers.h"


START_TEST(container_init_test)
{
        Con_Dynamic_Array example = init_dynamic_array(sizeof(int), NULL, NULL, NULL);
        ck_assert_int_eq(example.Block_Size, sizeof(int));
        ck_assert_ptr_eq(example.alloctaor.general_allocator, malloc);

        // failed init
        example = init_dynamic_array(1, NULL, NULL, &example);
        ck_assert_int_eq(example.Block_Size, 0);

} END_TEST

// Test for dynamic array insert
START_TEST(dynamic_array_insert_test)
{
        Con_Dynamic_Array example = init_dynamic_array(sizeof(int), malloc, free, NULL);
        int data = 42;

        dynamic_array_insert(&data, sizeof(int), &example);

        int *inserted_data = (int *)sequential_access_dynamic_array(&example);
        ck_assert_int_eq(*inserted_data, 42);

        // Check if Cur_Items has increased
        ck_assert_int_eq(example.Cur_Items, 1);
} END_TEST

// Test for dynamic array remove
START_TEST(dynamic_array_remove_test)
{
        Con_Dynamic_Array example = init_dynamic_array(sizeof(int), malloc, free, NULL);

        int data1 = 42, data2 = 73, data3 = 99;
        dynamic_array_insert(&data1, sizeof(int), &example);
        dynamic_array_insert(&data2, sizeof(int), &example);
        dynamic_array_insert(&data3, sizeof(int), &example);

        dynamic_array_remove(1, &example);

        // Check if the element at index 1 has been removed
        int *removed_data = (int *)random_access_dynamic_array(1, &example);
        ck_assert_int_eq(*removed_data, 99);

        // Check the number of current items
        ck_assert_int_eq(example.Cur_Items, 2);
} END_TEST

// Test for resetting sequential access dynamic array
START_TEST(reset_sequential_access_dynamic_array_test)
{
        Con_Dynamic_Array example = init_dynamic_array(sizeof(int), malloc, free, NULL);

        int data1 = 42, data2 = 73;
        dynamic_array_insert(&data1, sizeof(int), &example);
        dynamic_array_insert(&data2, sizeof(int), &example);

        reset_sequential_access_dynamic_array(&example);

        // Ensure that the sequential access pointer is reset
        int *seq_data = (int *)sequential_access_dynamic_array(&example);
        ck_assert_int_eq(*seq_data, 42);

        // Ensure Cur_Items remains unchanged
        ck_assert_int_eq(example.Cur_Items, 2);
} END_TEST

// Test for sequential access dynamic array
START_TEST(sequential_access_dynamic_array_test)
{
        Con_Dynamic_Array example = init_dynamic_array(sizeof(int), malloc, free, NULL);

        int data1 = 42;
        dynamic_array_insert(&data1, sizeof(int), &example);

        int *seq_data = (int *)sequential_access_dynamic_array(&example);
        ck_assert_int_eq(*seq_data, 42);
} END_TEST

// Test for random access dynamic array
START_TEST(random_access_dynamic_array_test)
{
        Con_Dynamic_Array example = init_dynamic_array(sizeof(int), malloc, free, NULL);

        int data1 = 42, data2 = 73;
        dynamic_array_insert(&data1, sizeof(int), &example);
        dynamic_array_insert(&data2, sizeof(int), &example);

        int *random_data = (int *)random_access_dynamic_array(0, &example);
        ck_assert_int_eq(*random_data, 42);
} END_TEST

// Test for finding an element in the dynamic array
START_TEST(find_dynamic_array_test)
{
        Con_Dynamic_Array example = init_dynamic_array(sizeof(int), malloc, free, NULL);

        int data1 = 42, data2 = 73;
        dynamic_array_insert(&data1, sizeof(int), &example);
        dynamic_array_insert(&data2, sizeof(int), &example);

        int search_data = 73;
        int found_index = find_dynamic_array(&search_data, &example);

        ck_assert_int_eq(found_index, 1);
} END_TEST

START_TEST(insert_linked_list_test)
{
        Con_Linked_List example = init_linked_list(NULL, NULL, NULL);
        int data1 = 69, data2 = 420;
        insert_linked_list(&data1, -1, &example);
        insert_linked_list(&data2, -1, &example);
        insert_linked_list(&data1, -1, &example);

        ck_assert_int_eq(*(int *)example.head->next->next->data, data1);
} END_TEST

START_TEST(remove_linked_list_test)
{
        Con_Linked_List example = init_linked_list(NULL, NULL, NULL);
        int data1 = 69, data2 = 420;
        insert_linked_list(&data1, 0, &example);
        remove_linked_list(0, &example);
        ck_assert_ptr_eq(example.head, NULL);

        insert_linked_list(&data1, 0, &example);
        insert_linked_list(&data2, 1, &example);
        insert_linked_list(&data1, 2, &example);

        remove_linked_list(1, &example);

        ck_assert_int_eq(*(int *)example.head->next->prev->data, data1);
} END_TEST

START_TEST(random_access_linked_list_test)
{
        Con_Linked_List example = init_linked_list(NULL, NULL, NULL);
        int data1 = 69, data2 = 420;
        insert_linked_list(&data1, 0, &example);
        insert_linked_list(&data2, 1, &example);
        insert_linked_list(&data1, 2, &example);

        ck_assert_int_eq(*(int *)random_access_linked_list(0, &example), data1);
        ck_assert_int_eq(*(int *)random_access_linked_list(1, &example), data2);
        ck_assert_int_eq(*(int *)random_access_linked_list(2, &example), data1);
} END_TEST

START_TEST(sequential_access_linked_list_test)
{
        Con_Linked_List example = init_linked_list(NULL, NULL, NULL);
        int data1 = 69, data2 = 420;
        insert_linked_list(&data1, -1, &example);
        insert_linked_list(&data2, -1, &example);
        insert_linked_list(&data1, -1, &example);

        ck_assert_int_eq(*(int *)sequential_access_linked_list(&example), data1);
        ck_assert_int_eq(*(int *)sequential_access_linked_list(&example), data2);
        ck_assert_int_eq(*(int *)sequential_access_linked_list(&example), data1);
        ck_assert_ptr_eq(sequential_access_linked_list(&example), (void *)0);
} END_TEST

START_TEST(push_stack_test)
{
        Con_Stack example = init_stack(NULL, NULL, NULL);
        int data1 = 69, data2 = 420;

        push_stack(&data1, &example);
        push_stack(&data2, &example);

        ck_assert_int_eq(*(int *)example.tail_node->data, data2);

        push_stack(&data1, &example);
        ck_assert_int_eq(*(int *)example.tail_node->data, data1);
} END_TEST

START_TEST(linked_list_empty_test)
{
        Con_Linked_List example = init_linked_list(NULL, NULL, NULL);
        ck_assert_ptr_eq(sequential_access_linked_list(&example), (void *)0);
        ck_assert_ptr_eq(random_access_linked_list(0, &example), (void *)0);
        ck_assert_int_eq(example.count, 0);
        free_linked_list(&example);
} END_TEST

START_TEST(linked_list_insert_at_positions_test)
{
        Con_Linked_List example = init_linked_list(NULL, NULL, NULL);
        int data1 = 42, data2 = 73, data3 = 99, data4 = 123;
        
        insert_linked_list(&data1, 0, &example);
        ck_assert_int_eq(*(int *)example.head->data, 42);
        ck_assert_int_eq(example.count, 1);
        
        insert_linked_list(&data2, 1, &example);
        ck_assert_int_eq(*(int *)example.head->next->data, 73);
        ck_assert_int_eq(example.count, 2);
        
        insert_linked_list(&data3, 1, &example);
        ck_assert_int_eq(*(int *)example.head->next->data, 99);
        ck_assert_int_eq(*(int *)example.head->next->next->data, 73);
        ck_assert_int_eq(example.count, 3);
        
        insert_linked_list(&data4, 10, &example);
        ck_assert_int_eq(example.count, 3);
        
        free_linked_list(&example);
} END_TEST

START_TEST(linked_list_remove_test)
{
        Con_Linked_List example = init_linked_list(NULL, NULL, NULL);
        int data1 = 42, data2 = 73, data3 = 99;
        
        insert_linked_list(&data1, 0, &example);
        insert_linked_list(&data2, 1, &example);
        insert_linked_list(&data3, 2, &example);
        
        remove_linked_list(0, &example);
        ck_assert_int_eq(*(int *)example.head->data, 73);
        ck_assert_int_eq(example.count, 2);
        remove_linked_list(1, &example);
        ck_assert_int_eq(*(int *)example.head->data, 73);
        ck_assert_int_eq(example.count, 1); 
        remove_linked_list(0, &example);
        ck_assert_ptr_eq(example.head, (void *)0);
        ck_assert_int_eq(example.count, 0);
               
        free_linked_list(&example);
} END_TEST

START_TEST(linked_list_sequential_access_test)
{
        Con_Linked_List example = init_linked_list(NULL, NULL, NULL);
        int data1 = 42, data2 = 73, data3 = 99;
        
        insert_linked_list(&data1, 0, &example);
        insert_linked_list(&data2, 1, &example);
        insert_linked_list(&data3, 2, &example);
        
        ck_assert_int_eq(*(int *)sequential_access_linked_list(&example), 42);
        ck_assert_int_eq(*(int *)sequential_access_linked_list(&example), 73);
        ck_assert_int_eq(*(int *)sequential_access_linked_list(&example), 99);
        ck_assert_ptr_eq(sequential_access_linked_list(&example), (void *)0);
        
        reset_sequential_access_linked_list(&example);
        ck_assert_int_eq(*(int *)sequential_access_linked_list(&example), 42);
        
        free_linked_list(&example);
} END_TEST

START_TEST(linked_list_random_access_test)
{
        Con_Linked_List example = init_linked_list(NULL, NULL, NULL);
        int data1 = 42, data2 = 73, data3 = 99;
        
        insert_linked_list(&data1, 0, &example);
        insert_linked_list(&data2, 1, &example);
        insert_linked_list(&data3, 2, &example);
        
        ck_assert_int_eq(*(int *)random_access_linked_list(0, &example), 42);
        ck_assert_int_eq(*(int *)random_access_linked_list(1, &example), 73);
        ck_assert_int_eq(*(int *)random_access_linked_list(2, &example), 99);
        
        ck_assert_ptr_eq(random_access_linked_list(-1, &example), (void *)0);
        ck_assert_ptr_eq(random_access_linked_list(3, &example), (void *)0);
        
        free_linked_list(&example);
} END_TEST

START_TEST(linked_list_custom_allocator_test)
{
        Con_Linked_List example = init_linked_list(malloc, free, NULL);
        int data1 = 42, data2 = 73;
        
        insert_linked_list(&data1, 0, &example);
        insert_linked_list(&data2, 1, &example);
        
        ck_assert_int_eq(*(int *)random_access_linked_list(0, &example), 42);
        ck_assert_int_eq(*(int *)random_access_linked_list(1, &example), 73);
        
        free_linked_list(&example);
} END_TEST
// Mocking memory allocation functions
void *custom_alloc(size_t size) {
        return malloc(size);
}

void custom_free(void *ptr) {
        free(ptr);
}

// Test suite for the queue functionality
START_TEST(test_queue_initially_empty) {
        // Initialize the queue
        Con_Queue queue = init_queue(custom_alloc, custom_free, NULL);

        // Assert that the queue is empty
        ck_assert_ptr_eq(queue.underlaying_data.linked_list.head, NULL);
        ck_assert_ptr_eq(queue.end_node, NULL);
}
END_TEST

START_TEST(test_enqueue_single_item) {
        // Initialize the queue
        Con_Queue queue = init_queue(custom_alloc, custom_free, NULL);

        // Enqueue a single item
        int value = 42;
        push_queue(&value, &queue);

        // Check the queue after enqueue
        ck_assert_ptr_ne(queue.underlaying_data.linked_list.head, NULL);  // Head should not be NULL after enqueue
        ck_assert_ptr_ne(queue.end_node, NULL);  // End node should also not be NULL
        ck_assert_int_eq(*(int *)queue.underlaying_data.linked_list.head->data, value);  // Check if value is enqueued correctly
}
END_TEST

START_TEST(test_dequeue_single_item) {
        // Initialize the queue
        Con_Queue queue = init_queue(custom_alloc, custom_free, NULL);

        // Enqueue a single item
        int value = 42;
        push_queue(&value, &queue);

        // Dequeue the single item
        int *dequeued_value = pop_queue(&queue);

        // Check the dequeued value
        ck_assert_int_eq(*dequeued_value, value);  // The value should match the enqueued value
        ck_assert_ptr_eq(queue.underlaying_data.linked_list.head, NULL);  // Queue should be empty again
        ck_assert_ptr_eq(queue.end_node, NULL);  // End node should be NULL after dequeue
}
END_TEST

START_TEST(test_enqueue_multiple_items) {
        // Initialize the queue
        Con_Queue queue = init_queue(custom_alloc, custom_free, NULL);

        // Enqueue multiple items
        int value1 = 42, value2 = 13, value3 = 99;
        push_queue(&value1, &queue);
        push_queue(&value2, &queue);
        push_queue(&value3, &queue);

        // Check if the first value is enqueued correctly
        ck_assert_int_eq(*(int *)queue.underlaying_data.linked_list.head->data, value1);

        // Check if the last value is the last enqueued value
        ck_assert_int_eq(*(int *)queue.end_node->data, value3);
}
END_TEST

START_TEST(test_dequeue_multiple_items) {
        // Initialize the queue
        
        Con_Queue queue = init_queue(custom_alloc, custom_free, NULL);

        // Enqueue multiple items
        int value1 = 42, value2 = 13, value3 = 99;
        push_queue(&value1, &queue);
        push_queue(&value2, &queue);
        push_queue(&value3, &queue);

        // Dequeue the first item
        int *dequeued_value = pop_queue(&queue);
        ck_assert_int_eq(*dequeued_value, value1);  // First item enqueued should be dequeued first

        // Dequeue the second item
        dequeued_value = pop_queue(&queue);
        ck_assert_int_eq(*dequeued_value, value2);  // Second item enqueued should be dequeued second

        // Dequeue the third item
        dequeued_value = pop_queue(&queue);
        ck_assert_int_eq(*dequeued_value, value3);  // Third item enqueued should be dequeued third
}END_TEST

START_TEST(test_dequeue_from_empty_queue) {
        // Initialize the queue
        Con_Queue queue = init_queue(custom_alloc, custom_free, NULL);

        // Dequeue from an empty queue (should return NULL)
        int *dequeued_value = pop_queue(&queue);
        ck_assert_ptr_eq(dequeued_value, NULL);  // Queue should be empty, so return NULL
}
END_TEST

Suite *container_test_suite()
{
        Suite *s;
        TCase *tc;

        s = suite_create("container_tests");
        tc = tcase_create("Core");

        tcase_add_test(tc, container_init_test);
        tcase_add_test(tc, container_init_test);
        tcase_add_test(tc, dynamic_array_insert_test);
        tcase_add_test(tc, dynamic_array_remove_test);
        tcase_add_test(tc, reset_sequential_access_dynamic_array_test);
        tcase_add_test(tc, sequential_access_dynamic_array_test);
        tcase_add_test(tc, random_access_dynamic_array_test);
        tcase_add_test(tc, find_dynamic_array_test);
        tcase_add_test(tc, insert_linked_list_test);
        tcase_add_test(tc, remove_linked_list_test);
        tcase_add_test(tc, random_access_linked_list_test);
        tcase_add_test(tc, sequential_access_dynamic_array_test);
        tcase_add_test(tc, push_stack_test);
        tcase_add_test(tc, linked_list_empty_test);
        tcase_add_test(tc, linked_list_insert_at_positions_test);
        tcase_add_test(tc, linked_list_remove_test);
        tcase_add_test(tc, linked_list_sequential_access_test);
        tcase_add_test(tc, linked_list_random_access_test);
        tcase_add_test(tc, linked_list_custom_allocator_test);
        tcase_add_test(tc, test_queue_initially_empty);
        tcase_add_test(tc, test_enqueue_single_item);
        tcase_add_test(tc, test_dequeue_single_item);
        tcase_add_test(tc, test_enqueue_multiple_items);
        tcase_add_test(tc, test_dequeue_multiple_items);
        tcase_add_test(tc, test_dequeue_from_empty_queue);

        suite_add_tcase(s, tc);

        return s;
}

void run_container_tests()
{
        int failed = 0;
        int run = 0;
        Suite *s;
        SRunner *runner;

        printf("running container tests\n");

        s = container_test_suite();
        runner = srunner_create(s);

        srunner_run_all(runner, CK_NORMAL);

        run = srunner_ntests_run(runner);
        failed = srunner_ntests_failed(runner);

        srunner_free(runner);
        printf("\n%d of %d Passed\n", run-failed, run);
}

int main(int argc, char *argv[])
{
        run_container_tests();
        return 0;
}
