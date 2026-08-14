#include <assert.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <utils.h>
#include <unistd.h>
#include <test_common.h>
#include <test_stats.h>

test_runner_t *tests;
bool has_pipe = false;

void prepare_tests();
void test_octascii_to_dec(unsigned int id);

unsigned int number_of_tests = 0;
int main(int argc, char **argv) {
    int pipe_fd = -1;
    if (argc > 1) {
            //pipe_fd = atoi(argv[1]);
            has_pipe = true;
            pipe_fd = atoi(argv[1]);
            printf("Found pipe id: %d\n", pipe_fd);
    }
    prepare_tests(pipe_fd);
    tests[0].handler(0);
    print_stats(tests[0].stats);
    print_stats(tests[0].stats);
    if ( has_pipe ) {
        write(pipe_fd, &number_of_tests, sizeof(number_of_tests));
        send_stats(pipe_fd, tests[0].stats);
        close(pipe_fd);
    }
    printf("\n");
}

void prepare_tests() {
    printf("Testing Utility functions  \n");
    printf("===============================\n\n");
    test_init(1, &tests);
    add_test(1, "Testing octal to decimal functions", test_octascii_to_dec, tests);
    number_of_tests++;    
}


void test_octascii_to_dec(unsigned int id) {
    printf("%d) %s:\n", tests[id].stats.module_id, tests[id].stats.module_title);
    char filesize[12] = {'0','0','0','0','0','0','1','3','3','3','6','0'};
    int result = octascii_to_dec(filesize, 12);
    pretty_assert_stat(5854, result, ==, tests[id].stats,"Testing to convert an octal ascii number to int", has_pipe);
    //pretty_assert(5854, result, ==, "Testing to convert an octal ascii number to int");
    char filesize_zero[12] = {'0','0','0','0','0','0','0','0','0','0','0','0'};
    result = octascii_to_dec(filesize_zero, 12);
    pretty_assert_stat(0, result, ==, tests[id].stats,"Testing to convert an octal ascii number to int", has_pipe);
    //pretty_assert(0, result, ==, "Testing to convert an octal ascii number to int");
    char filesize_tt[12] = {'0','0','0','0','0','0','2','7','3','4','0', '0'};
    result = octascii_to_dec(filesize_tt, 12);
    pretty_assert_stat(12000, result, ==, tests[id].stats, "Testing to convert an octal ascii number to int", has_pipe);
    //pretty_assert(12000, result, ==, "Testing to convert an octal ascii number to int");
}
