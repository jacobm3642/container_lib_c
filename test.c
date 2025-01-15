#include <check.h>
#include <stdio.h>

#include "containers.h"


START_TEST(example_test)
{
} END_TEST


Suite *container_test_suite()
{
        Suite *s;
        TCase *tc;

        s = suite_create("container_tests");
        tc = tcase_create("Core");

        tcase_add_test(tc, example_test);

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
