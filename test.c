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
