#include <test_common.h>
#include <assert.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <ustar.h>

void test_is_zeroed();

ustar_item zero_item = {0};
ustar_item tar_item = {0};

int main() {
    printf("Testing USTAR Utility function  -\n");
    printf("===============================\n\n");
    strcpy(tar_item.magic, "ustar");
    test_is_zeroed();
    printf("\n");
}


void test_is_zeroed() {
    printf("Testing Tar functions\n");
    bool result = tar_is_zeroed(&zero_item);
    pretty_assert(true, result, ==, "Testing tar_is_zeroed with an item set to zero");
    result = tar_is_zeroed(&tar_item);
    pretty_assert(false, result, ==, "Testing tar_is_zeroed with an item not zero");
}
