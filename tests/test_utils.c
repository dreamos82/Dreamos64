#include <assert.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <utils.h>
#include <unistd.h>
#include <test_common.h>
#include <test_stats.h>

unsigned int number_of_tests = 0;
test_runner_t *tests;
bool has_pipe = false;

void prepare_tests();
void test_octascii_to_dec(unsigned int id);

int main(int argc, char **argv) {
    int pipe_fd = -1;    
    if (argc > 1) {
            //pipe_fd = atoi(argv[1]);
            has_pipe = true;
            pipe_fd = atoi(argv[1]);
            printf("Found pipe id: %d\n", pipe_fd);
    }
    prepare_tests();
    printf("%d) %s:\n", tests[0].stats.module_id, tests[0].stats.module_title);
    tests[0].handler(0);
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
    char filesize[12] = {'0','0','0','0','0','0','1','3','3','3','6','0'};
    int result = octascii_to_dec(filesize, 12);
    pretty_assert_stat(5854, result, ==, tests[id].stats,"Testing to convert an octal ascii number to int", has_pipe);
    char filesize_zero[12] = {'0','0','0','0','0','0','0','0','0','0','0','0'};
    result = octascii_to_dec(filesize_zero, 12);
    pretty_assert_stat(0, result, ==, tests[id].stats,"Testing to convert an octal ascii number to int", has_pipe);
    char filesize_tt[12] = {'0','0','0','0','0','0','2','7','3','4','0', '0'};
    result = octascii_to_dec(filesize_tt, 12);
    pretty_assert_stat(12000, result, ==, tests[id].stats, "Testing to convert an octal ascii number to int", has_pipe);
}
