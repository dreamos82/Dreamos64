#include <test_common.h>
#include <base/numbers.h>
#include <string.h>
#include <stdio.h>
#include <assert.h>

int main(){
    printf("-----Testing Numbers conversion functions----\n");
    printf("--- Testing GetDecString ---\n");
    char test_buffer[5];
    int test_size = _getDecString(test_buffer, 250);
    printf("--Buffer should contain 250 and size should be 3: %s - %d\n", test_buffer, test_size);
    assert(test_size == 3);
    assert(strcmp(test_buffer, "250") == 0);
    test_size = _getDecString(test_buffer, 1530);
    printf("--Buffer should contain 1530 and size should be 4: %s - %d\n", test_buffer, test_size);
    assert(test_size == 4);
    assert(strcmp(test_buffer, "1530") == 0);
    printf("--- Testing GetHexString ---\n");
    test_size = _getHexString(test_buffer, 0x9AB, false);
    printf("--Buffer should contain 9AB and size should be 2: %s - %d\n", test_buffer, test_size);
    assert(test_size == 3);
    assert(strcmp(test_buffer, "9AB") == 0);
    test_size = _getHexString(test_buffer, 0x2A, false);
    printf("--Buffer should contain 2A and size should be 1: %s - %d\n", test_buffer, test_size);
    assert(test_size == 2);
    assert(strcmp(test_buffer, "2A") == 0);

}
