#include <base/numbers.h>
#include <limits.h>
#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <test_common.h>
#include <test_stats.h>
#include <unistd.h>

void test_decimal_conversion(unsigned int id);
void test_hex_conversion(unsigned int id);
void prepare_tests();

bool has_pipe = false;
test_runner_t *tests;
unsigned int number_of_tests = 0;

int main(int argc, char **argv){
    int pipe_fd = -1;
    if (argc > 1) {
            //pipe_fd = atoi(argv[1]);
            has_pipe = true;
            pipe_fd = atoi(argv[1]);
            printf("Found pipe id: %d\n", pipe_fd);
    }
    prepare_tests();
    tests[0].handler(0);
    tests[1].handler(1);
    print_stats(tests[0].stats);
    print_stats(tests[1].stats);
    if ( has_pipe ) {
        write(pipe_fd, &number_of_tests, sizeof(number_of_tests));
        send_stats(pipe_fd, tests[0].stats);
        send_stats(pipe_fd, tests[1].stats);
        close(pipe_fd);
    }
}

void prepare_tests() {
    printf("=Testing Numbers conversion functions -\n");
    printf("=================================\n\n");
    test_init(2, &tests);
    add_test(1, "Testing decimal conversion", test_decimal_conversion, tests);
    number_of_tests++;
    add_test(2, "Testing hexadeciaml conversion", test_hex_conversion, tests);
    number_of_tests++;
}

void test_decimal_conversion(unsigned int id) {
    printf("%d) %s:\n", tests[id].stats.module_id, tests[id].stats.module_title);
    char test_buffer[6];
    int test_size = _getDecString(test_buffer, 250);
    pretty_assert_stat(3, test_size, ==, tests[id].stats, "Test with value 250, returned size:", has_pipe);
    pretty_assert_stat(0, strcmp(test_buffer, "250"), ==, tests[id].stats, "test_buffer should contain 250 with strcmp", has_pipe);
    test_size = _getDecString(test_buffer, 1530);
    pretty_assert_stat(4, test_size, ==, tests[id].stats, "Test with value 1530, returned size", has_pipe);
    pretty_assert_stat(0, strcmp(test_buffer, "1530"), ==, tests[id].stats, "test_buffer should contain 1530 with strcmp", has_pipe);
    test_size = _getDecString(test_buffer, -5);
    pretty_assert_stat(2, test_size, ==, tests[id].stats, "Test with value -5", has_pipe);
    pretty_assert_stat(0, strcmp(test_buffer, "-5"), ==, tests[id].stats, "test_buffer should contain the value -5 with strcmp", has_pipe);
    test_size = _getDecString(test_buffer, 54321);
    pretty_assert_stat(5, test_size, ==, tests[id].stats, "testing with value 54321, returned size", has_pipe);
    pretty_assert_stat(0, strcmp(test_buffer, "54321"), ==, tests[id].stats, "test_buffer should contain 54321", has_pipe);
    char test_buffer_long[15];
    test_size = _getDecString(test_buffer_long, -2147483648);
    pretty_assert_stat(11, test_size, ==, tests[id].stats, "Testing with long number returned size", has_pipe);
    pretty_assert_stat(0, strcmp(test_buffer_long, "-2147483648"), ==, tests[id].stats, "test_buffer should contain -2147483648 with strcmp", has_pipe);
    test_size = _getDecString(test_buffer_long, 2147483648);

    pretty_assert_stat(10, test_size, ==, tests[id].stats, "Testing with 2147483648, test_size value", has_pipe);
    pretty_assert_stat(0, strcmp(test_buffer_long, "2147483648"), ==, tests[id].stats, "test_buffer should contain: 2147483648", has_pipe);
    test_size = _getUnsignedDecString(test_buffer_long, 4294967295);
    pretty_assert_stat(10, test_size,  ==, tests[id].stats, "Testing with 4294967295 test_size value", has_pipe);
    pretty_assert_stat(0, strcmp(test_buffer_long, "4294967295"),  ==, tests[id].stats, "test_buffer should contain 4294967295", has_pipe);
}

void test_hex_conversion(unsigned int id){
    printf("%d) %s:\n", tests[id].stats.module_id, tests[id].stats.module_title);
    char test_buffer[6];
    int test_size = _getHexString(test_buffer, 0x9AB, true);
    pretty_assert_stat(3, test_size, ==, tests[id].stats, "test hex conversion with value 0x9AB returned size", has_pipe);
    pretty_assert_stat(0, strcmp(test_buffer, "9AB"), ==, tests[id].stats, " test_buffer should contain 9AB", has_pipe);
    test_size = _getHexString(test_buffer, 0x2A, false);
    pretty_assert_stat(2, test_size, ==, tests[id].stats, "Testing with 0x2A, test_size value", has_pipe);
    pretty_assert_stat(0, strcmp(test_buffer, "2a"), ==, tests[id].stats, "test_buffer should contain 2a", has_pipe);    
}
