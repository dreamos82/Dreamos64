#include <assert.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <utils.h>
#include <test_common.h>

void test_octascii_to_dec();

int main(){
    printf("Testing Utility functions  -\n");
    printf("===============================\n\n");
    test_octascii_to_dec();
    printf("\n");
}


void test_octascii_to_dec() {
    printf("\nTesting octal to decimal functions\n");
    char filesize[12] = {'0','0','0','0','0','0','1','3','3','3','6','0'};
    int result = octascii_to_dec(filesize, 12);
    pretty_assert(5854, result, ==, "Testing to convert an octal ascii number to int");
    char filesize_zero[12] = {'0','0','0','0','0','0','0','0','0','0','0','0'};
    result = octascii_to_dec(filesize_zero, 12);
    pretty_assert(0, result, ==, "Testing to convert an octal ascii number to int");
    char filesize_tt[12] = {'0','0','0','0','0','0','2','7','3','4','0', '0'};
    result = octascii_to_dec(filesize_tt, 12);
    pretty_assert(12000, result, ==, "Testing to convert an octal ascii number to int");
}
