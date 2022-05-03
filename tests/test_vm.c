#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <assert.h>
#include <vm.h>


void test_is_address_higher_half();

int main() {
    test_is_address_higher_half();
}
    

void test_is_address_higher_half() {
    printf("Testing is address higher_half\n");

    bool is_hh = is_address_higher_half(0x100000);
    printf("--- Should return false (0) for 0x100000 - %d\n", is_hh);
    assert(is_hh == false);
    is_hh = is_address_higher_half(0xffff100000);
    printf("--- Should return false (0) for 0xffff100000 - %d\n", is_hh);
    assert(is_hh == false);
    is_hh = is_address_higher_half(0xFFFF800000000000);
    printf("--- Should return false (0) 0xFFFF800000000000 -  %d\n", is_hh);
    assert(is_hh == true);


}
