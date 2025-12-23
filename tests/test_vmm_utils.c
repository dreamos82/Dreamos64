#include <test_common.h>
#include <vmm_util.h>
#include <assert.h>
#include <stdio.h>

void test_utils();

int main() {
    printf("Testing VMM Utility function  -\n");
    printf("===============================\n\n");
    test_utils();
    printf("\n");
}

void test_utils() {
    size_t number_of_pages = get_number_of_pages_from_size(0x900);
    pretty_assert(1, number_of_pages, ==, "Testing number of pages for 0x900");
    number_of_pages = get_number_of_pages_from_size(0x0);
    pretty_assert(0, number_of_pages, ==, "Testing number of pages for 0x0");
    pretty_assert(0x200000, align_value_to_page(0x100), ==, "Testing alignment for for 0x100");
    pretty_assert(0x400000, align_value_to_page(0x200015), ==, "Testing number of pages for 0x200015");
    pretty_assert(0x200000, align_down(0x3c7000, 0x200000), ==, "Testing align_down");
    pretty_assert(0x600000, align_down(0x6c7000, 0x200000), ==, "Testing align_down");
}
