#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <test_common.h>
#include <test_stats.h>
#include <unistd.h>
#include <vmm_util.h>

void test_utils();
void prepare_tests(int pipe_fd);

bool has_pipe = false;
test_runner_t *tests;
unsigned int number_of_tests =0;

int main(int argc, char **argv) {
    int pipe_fd = -1;
     if (argc > 1) {
            //pipe_fd = atoi(argv[1]);
            has_pipe = true;
            pipe_fd = atoi(argv[1]);
            printf("Found pipe id: %d\n", pipe_fd);
    }
    prepare_tests(pipe_fd);
    tests[0].handler(pipe_fd);
    if ( has_pipe ) {
        write(pipe_fd, &number_of_tests, sizeof(number_of_tests));
        send_stats(pipe_fd, tests[0].stats);
        close(pipe_fd);
    }
    //test_utils();
    printf("\n");
}

void prepare_tests(int pipe_fd) {
    printf("Testing VMM Utility function  -\n");
    printf("=================================\n\n");
    test_init(1, &tests);
    add_test(1, "Testing vnn utils functions", test_utils, tests);
    number_of_tests++;
}

void test_utils() {
    size_t number_of_pages = get_number_of_pages_from_size(0x900);
    pretty_assert(1, number_of_pages, ==, "Testing number of pages for 0x900");
    pretty_assert_stat(1, number_of_pages, ==, tests[0].stats, "Testing number of pages for 0x900");
    number_of_pages = get_number_of_pages_from_size(0x0);
    pretty_assert(0, number_of_pages, ==, "Testing number of pages for 0x0");
    pretty_assert(0x200000, align_value_to_page(0x100), ==, "Testing alignment for for 0x100");
    pretty_assert(0x400000, align_value_to_page(0x200015), ==, "Testing number of pages for 0x200015");
    pretty_assert(0x200000, align_down(0x3c7000, 0x200000), ==, "Testing align_down");
    pretty_assert(0x600000, align_down(0x6c7000, 0x200000), ==, "Testing align_down");
    pretty_assert_stat(0, number_of_pages, ==, tests[0].stats, "Testing number of pages for 0x0");
    pretty_assert_stat(0x200000, align_value_to_page(0x100), ==, tests[0].stats, "Testing alignment for for 0x100");
    pretty_assert_stat(0x400000, align_value_to_page(0x200015), ==, tests[0].stats, "Testing number of pages for 0x200015");
    pretty_assert_stat(0x200000, align_down(0x3c7000, 0x200000), ==, tests[0].stats, "Testing align_down");
    pretty_assert_stat(0x600000, align_down(0x6c7000, 0x200000), ==, tests[0].stats, "Testing align_down");
}
