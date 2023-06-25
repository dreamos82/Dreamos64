#include <test_common.h>
#include <base/numbers.h>
#include <string.h>
#include <stdio.h>
#include <assert.h>
#include <limits.h>

int main(){
    printf("Testing Numbers conversion functions\n");
    printf("\t [test_number_conversion][GetDecString]: Testing GetDecString ---\n");
    char test_buffer[6];
    int test_size = _getDecString(test_buffer, 250);
    printf("\t [test_number_conversion][GetDecString]: Buffer should contain 250 and size should be 3: %s - %d\n", test_buffer, test_size);
    assert(test_size == 3);
    assert(strcmp(test_buffer, "250") == 0);
    test_size = _getDecString(test_buffer, 1530);
    printf("\t [test_number_conversion][GetDecString]: Buffer should contain 1530 and size should be 4: %s - %d\n", test_buffer, test_size);
    assert(test_size == 4);
    assert(strcmp(test_buffer, "1530") == 0);
    test_size = _getDecString(test_buffer, -5);
    printf("\t [test_number_conversion][GetDecString]: Buffer should contain -5 and size should be 2: %s - %d\n", test_buffer, test_size);
    assert(test_size == 2);
    assert(strcmp(test_buffer, "-5") == 0);
    test_size = _getDecString(test_buffer, 54321);
    printf("\t [test_number_conversion][GetDecString]: Buffer should contain 54321 and size should be 5: %s - %d\n", test_buffer, test_size);
    assert(test_size == 5);
    assert(strcmp(test_buffer, "54321") == 0);
    char test_buffer_long[15];
    test_size = _getDecString(test_buffer_long, -2147483648);
    printf("\t [test_number_conversion][GetDecString]: Buffer should contain -2147483648 and size should be 11: %s - %d\n", test_buffer_long, test_size);
    assert(test_size == 11);
    assert(strcmp(test_buffer_long, "-2147483648") == 0);
    printf("Testing GetHexString\n");
    test_size = _getHexString(test_buffer, 0x9AB, true);
    printf("\t [test_number_conversion][GetHexString]: Buffer should contain 9AB and size should be 2: %s - %d\n", test_buffer, test_size);
    assert(test_size == 3);
    assert(strcmp(test_buffer, "9AB") == 0);
    test_size = _getHexString(test_buffer, 0x2A, false);
    printf("\t [test_number_conversion][GetHexString]: Buffer should contain 2a and size should be 1: %s - %d\n", test_buffer, test_size);
    assert(test_size == 2);
    assert(strcmp(test_buffer, "2a") == 0);
    test_size = _getDecString(test_buffer_long, 2147483648);
    printf("\t [test_number_conversion][GetHexString]: Buffer should contain 2147483648 and size should be 10: %s - %d\n", test_buffer_long, test_size);
    assert(test_size == 10);
    assert(strcmp(test_buffer_long, "2147483648") == 0);
    printf("Testing GedUnsignedDecString\n");
    test_size = _getUnsignedDecString(test_buffer_long, 4294967295);
    printf("\t [test_number_conversion][UnsignedGetDecString]:  Buffer should contain 4294967295 and size should be 10: %s - %d\n", test_buffer_long, test_size);
    assert(test_size == 10);
    assert(strcmp(test_buffer_long, "4294967295") == 0);
}
