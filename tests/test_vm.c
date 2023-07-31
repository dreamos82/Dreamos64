#include <test_common.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <assert.h>
#include <vm.h>
#include <vmm_util.h>


void test_is_address_higher_half();
void test_vm_parse_flags();

typedef enum {
    VMM_FLAGS_NONE = 0,
    VMM_FLAGS_PRESENT = (1 << 0),
    VMM_FLAGS_WRITE_ENABLE = (1 << 1),
    VMM_FLAGS_USER_LEVEL = (1 << 2),
    VMM_FLAGS_ADDRESS_ONLY = (1 << 7)
} paging_flags_t;

int main() {
    test_is_address_higher_half();
    test_vm_parse_flags();
}
    

void test_is_address_higher_half() {
    printf("Testing is address higher_half\n");

    bool is_hh = is_address_higher_half(0x100000);
    printf("\t [test_vm](is_address_higher_half): Should return false (0) for 0x100000 - %d\n", is_hh);
    assert(is_hh == false);
    is_hh = is_address_higher_half(0xffff100000);
    printf("\t [test_vm](is_address_higher_half): Should return false (0) for 0xffff100000 - %d\n", is_hh);
    assert(is_hh == false);
    is_hh = is_address_higher_half(0xFFFF800000000000);
    printf("\t [test_vm](is_address_higher_half): Should return false (0) 0xFFFF800000000000 -  %d\n", is_hh);
    assert(is_hh == true);
}

void test_vm_parse_flags() {
        printf("Testing test_vm_parse_flags\n");
        printf("\t[test_vm](%s): Should return 0 - %d\n", __FUNCTION__, vm_parse_flags(VMM_FLAGS_ADDRESS_ONLY));
        assert(0 == vm_parse_flags(VMM_FLAGS_ADDRESS_ONLY));
        printf("\t[test_vm](%s): Should return 3 - %d\n", __FUNCTION__, vm_parse_flags(VMM_FLAGS_ADDRESS_ONLY | VMM_FLAGS_PRESENT | VMM_FLAGS_WRITE_ENABLE));
        assert(3 == vm_parse_flags(VMM_FLAGS_ADDRESS_ONLY | VMM_FLAGS_PRESENT | VMM_FLAGS_WRITE_ENABLE));
        printf("\t[test_vm](%s): Should return 2 - %d\n", __FUNCTION__, vm_parse_flags(VMM_FLAGS_WRITE_ENABLE));
        assert(2 == vm_parse_flags( VMM_FLAGS_WRITE_ENABLE));
        printf("\t[test_vm](%s): Should return 3 - %d\n", __FUNCTION__, vm_parse_flags(VMM_FLAGS_PRESENT | VMM_FLAGS_WRITE_ENABLE));
        assert(3 == vm_parse_flags(VMM_FLAGS_ADDRESS_ONLY | VMM_FLAGS_PRESENT | VMM_FLAGS_WRITE_ENABLE));
}
