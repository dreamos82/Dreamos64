#include <vmm_util.h>
#include <assert.h>
#include <stdio.h>

int main() {
    printf("Testing VMM Utility function  -\n");
    size_t number_of_pages = get_number_of_pages_from_size(0x900);
    printf("\t [test_utils] (get_number_of_pages_from_size): Testing number of pages for 0x900, should be 1: %d\n", number_of_pages);
    assert(number_of_pages == 1);
    number_of_pages = get_number_of_pages_from_size(0x0);
    printf("\t [test_utils] (get_number_of_pages_from_size): Testing number of pages for 0x0, should be 0: %d\n", number_of_pages);
    assert(align_value_to_page(0x100) == 0x200000);
    printf("\t [test_utils] (align_value_to_page): Testing alignment for 0x100, should be 0x200000: %x\n", align_value_to_page(0x100));
    assert(align_value_to_page(0x200015) == 0x400000);
    printf("\t [test_utils] (align_value_to_page):  Testing alignment for 0x100, should be 0x200015: %x\n", align_value_to_page(0x200015));

}
