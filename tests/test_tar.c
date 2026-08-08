#include <test_common.h>
#include <test_stats.h>
#include <assert.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <ustar.h>

void test_is_zeroed(int pipe_fd);
void test_get_file_start(int pipe_fd);
void prepare_tests(int pipe_fd);

ustar_item zero_item = {0};
ustar_item tar_item = {0};
ustar_item example_item = {0};

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
    strcpy(tar_item.magic, "ustar");
    prepare_tests(pipe_fd);        
    tests[0].handler(pipe_fd);
    tests[1].handler(pipe_fd);
    if ( has_pipe ) {
        write(pipe_fd, &number_of_tests, sizeof(number_of_tests));
        send_stats(pipe_fd, tests[0].stats);
        send_stats(pipe_fd, tests[1].stats);
        close(pipe_fd);
    }
    printf("\n");
}

void prepare_tests(int pipe_fd) {
    printf("=Testing USTAR Utility function -\n");
    printf("=================================\n\n");
    test_init(2, &tests);
    add_test(1, "Testing Tar functions", test_is_zeroed, tests);
    number_of_tests++;
    add_test(2, "Testing Tar get_file_start function", test_get_file_start, tests);
    number_of_tests++;
}

void test_is_zeroed(int pipe_fd) {
    printf("Testing Tar functions\n");
    bool result = ustar_is_zeroed(&zero_item);
    pretty_assert(true, result, ==, "Testing tar_is_zeroed with an item set to zero");
    pretty_assert_stat(true, result, ==, tests[0].stats, "Testing tar_is_zeroed with an item set to zero");
    result = ustar_is_zeroed(&tar_item);
    pretty_assert(false, result, ==, "Testing tar_is_zeroed with an item not zero");
    pretty_assert_stat(false, result, ==, tests[0].stats, "Testing tar_is_zeroed with an item not zero");
}

void test_get_file_start(int pipe_fd) {
    printf("Testing tar get_file_start function\n");
    pretty_assert(NULL, ustar_get_file_start(NULL),  ==, "Testing get_file_start with NULL value");
    pretty_assert_stat(NULL, ustar_get_file_start(NULL),  ==, tests[1].stats, "Testing get_file_start with NULL value");
    char *expected_result = (char *)(&example_item)+512;
    pretty_assert(expected_result, (char *) ustar_get_file_start(&example_item),  ==, "Testing get_file_start with example_item value");
    pretty_assert_stat(expected_result, (char *) ustar_get_file_start(&example_item),  ==, tests[1].stats, "Testing get_file_start with example_item value");
}
