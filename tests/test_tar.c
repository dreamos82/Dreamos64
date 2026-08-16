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

void test_is_zeroed(unsigned int id);
void test_get_file_start(unsigned int id);
void prepare_tests();

ustar_item zero_item = {0};
ustar_item tar_item = {0};
ustar_item example_item = {0};

bool has_pipe = false;
test_runner_t *tests;
unsigned int number_of_tests = 0;

int main(int argc, char **argv) {
    int pipe_fd = -1;
    if (argc > 1) {
            //pipe_fd = atoi(argv[1]);
            has_pipe = true;
            pipe_fd = atoi(argv[1]);
            printf("Found pipe id: %d\n", pipe_fd);
    }
    strcpy(tar_item.magic, "ustar");
    prepare_tests();
    for (unsigned int i = 0; i < number_of_tests; i++) {
        tests[i].handler(i);        
        print_stats(tests[i].stats);
    }
    if ( has_pipe ) {
        write(pipe_fd, &number_of_tests, sizeof(number_of_tests));
        send_stats(pipe_fd, tests[0].stats);
        send_stats(pipe_fd, tests[1].stats);
        close(pipe_fd);
    }
    printf("\n");
}

void prepare_tests() {
    printf("=Testing USTAR Utility function -\n");
    printf("=================================\n\n");
    test_init(2, &tests);
    add_test(1, "Testing Tar functions", test_is_zeroed, tests);
    number_of_tests++;
    add_test(2, "Testing Tar get_file_start function", test_get_file_start, tests);
    number_of_tests++;
}

void test_is_zeroed(unsigned int id) {
    printf("%d) %s:\n", tests[id].stats.module_id, tests[id].stats.module_title);
    bool result = ustar_is_zeroed(&zero_item);
    pretty_assert_stat(true, result, ==, tests[id].stats, "Testing tar_is_zeroed with an item set to zero", has_pipe);
    result = ustar_is_zeroed(&tar_item);
    pretty_assert_stat(false, result, ==, tests[id].stats, "Testing tar_is_zeroed with an item not zero", has_pipe);
}

void test_get_file_start(unsigned int id) {
    printf("%d) %s:\n", tests[id].stats.module_id, tests[id].stats.module_title);
    pretty_assert_stat(NULL, ustar_get_file_start(NULL),  ==, tests[id].stats, "Testing get_file_start with NULL value", has_pipe);
    char *expected_result = (char *)(&example_item)+512;
    pretty_assert_stat(expected_result, (char *) ustar_get_file_start(&example_item),  ==, tests[id].stats, "Testing get_file_start with example_item value", has_pipe);    
}
