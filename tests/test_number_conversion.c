#include <test_common.h>
#include <base/numbers.h>
#include <string.h>
#include <stdio.h>
#include <assert.h>
#include <limits.h>

void test_decimal_conversion();
void test_hex_conversion();

int main(){
    printf("Testing Numbers conversion functions\n");
    test_decimal_conversion();
    test_hex_conversion();
}

void test_decimal_conversion() {
    char test_buffer[6];
    int test_size = _getDecString(test_buffer, 250);
    pretty_assert(3, test_size, ==, "Test with value 250, returned size:");
    //pretty_assert(true, result, ==, "Testing tar_is_zeroed with an item set to zero");
    //pretty_assert_stat(true, result, ==, tests[0].stats, "Testing tar_is_zeroed with an item set to zero");
    pretty_assert(0, strcmp(test_buffer, "250"), ==, "test_buffer should contain 250 with strcmp");    
    test_size = _getDecString(test_buffer, 1530);
    pretty_assert(4, test_size, ==, "Test with value 1530, returned size");
    pretty_assert(0, strcmp(test_buffer, "1530"), ==, "test_buffer should contain 1530 with strcmp");
    test_size = _getDecString(test_buffer, -5);
    pretty_assert(2, test_size, ==, "Test with value -5");
    pretty_assert(0, strcmp(test_buffer, "-5"), ==, "test_buffer should contain the value -5 with strcmp");
    test_size = _getDecString(test_buffer, 54321);
    pretty_assert(5, test_size, ==, "testing with value 54321, returned size");
    pretty_assert(0, strcmp(test_buffer, "54321"), ==, "test_buffer should contain 54321");
    char test_buffer_long[15];
    test_size = _getDecString(test_buffer_long, -2147483648);
    pretty_assert(11, test_size, ==, "Testing with long number returned size");
    pretty_assert(0, strcmp(test_buffer_long, "-2147483648"), ==, "test_buffer should contain -2147483648 with strcmp");
    test_size = _getDecString(test_buffer_long, 2147483648);
    pretty_assert(10, test_size, ==, "Testing with 2147483648, test_size value");
    pretty_assert(0, strcmp(test_buffer_long, "2147483648"), ==, "test_buffer should contain: 2147483648");
    test_size = _getUnsignedDecString(test_buffer_long, 4294967295);
    printf("\t [test_number_conversion][UnsignedGetDecString]:  Buffer should contain 4294967295 and size should be 10: %s - %d\n", test_buffer_long, test_size);
    pretty_assert(10, test_size,  ==,  "Testing with 4294967295 test_size value");
    pretty_assert(0, strcmp(test_buffer_long, "4294967295"),  ==, "test_buffer should contain 4294967295");
}

void test_hex_conversion(){
    printf("Testing GetHexString\n");
    char test_buffer[6];
    int test_size = _getHexString(test_buffer, 0x9AB, true);
    pretty_assert(3, test_size, ==, "test hex conversion with value 0x9AB returned size");
    pretty_assert(0, strcmp(test_buffer, "9AB"), ==, " test_buffer should contain 9AB");
    test_size = _getHexString(test_buffer, 0x2A, false);    
    pretty_assert(2, test_size, ==, "Testing with 0x2A, test_size value");
    pretty_assert(0, strcmp(test_buffer, "2a"), ==, "test_buffer should contain 2a");
    printf("Testing GedUnsignedDecString\n");
}
