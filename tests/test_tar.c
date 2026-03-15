#include <test_common.h>
#include <assert.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <ustar.h>

void test_is_zeroed();
void test_get_file_start();

ustar_item zero_item = {0};
ustar_item tar_item = {0};
ustar_item example_item = {0};

int main() {
    printf("Testing USTAR Utility function  -\n");
    printf("===============================\n\n");
    strcpy(tar_item.magic, "ustar");
    test_is_zeroed();
    test_get_file_start();
    printf("\n");
}


void test_is_zeroed() {
    printf("Testing Tar functions\n");
    bool result = ustar_is_zeroed(&zero_item);
    pretty_assert(true, result, ==, "Testing tar_is_zeroed with an item set to zero");
    result = ustar_is_zeroed(&tar_item);
    pretty_assert(false, result, ==, "Testing tar_is_zeroed with an item not zero");
}

void test_get_file_start() {
    printf("Testing tar get_file_start function\n");
    pretty_assert(NULL, ustar_get_file_start(NULL),  ==, "Testing get_file_start with NULL value");
    char *expected_result = (char *)(&example_item)+512;
    pretty_assert(expected_result, (char *) ustar_get_file_start(&example_item),  ==, "Testing get_file_start with example_item value");
}
