#include "include/test_stats.h"
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <test_common.h>
#include <test_stats.h>
#include <unistd.h>
#include <vm.h>
#include <vmm_util.h>

test_runner_t *tests;
bool has_pipe = false;
unsigned int number_of_tests = 0;

void test_ensure_address_in_higher_half(unsigned int id);
void test_is_address_higher_half(unsigned int id);
void test_vm_parse_flags(unsigned int id);
void prepare_tests();

typedef enum {
    VMM_FLAGS_NONE = 0,
    VMM_FLAGS_PRESENT = (1 << 0),
    VMM_FLAGS_WRITE_ENABLE = (1 << 1),
    VMM_FLAGS_USER_LEVEL = (1 << 2),
    VMM_FLAGS_ADDRESS_ONLY = (1 << 7)
} paging_flags_t;

int main(int argc, char **argv) {    
    int pipe_fd = -1;
    if (argc > 1) {
            //pipe_fd = atoi(argv[1]);
            has_pipe = true;
            pipe_fd = atoi(argv[1]);
            printf("Found pipe id: %d\n", pipe_fd);
    }
    prepare_tests();
    for (int i=0; i < number_of_tests; i++) {
        printf("%d) %s:\n", tests[i].stats.module_id, tests[i].stats.module_title);
        tests[i].handler(i);
        print_stats(tests[i].stats);
    }
    if (has_pipe){
        write(pipe_fd, &number_of_tests, sizeof(number_of_tests));
        for(int j=0; j < number_of_tests; j++) {
            send_stats(pipe_fd, tests[j].stats);
            close(pipe_fd);
        }
    }
    printf("\n");
}

void prepare_tests(){
    printf("Testing Virtual Memory functions\n");
    printf("===============================\n\n");
    test_init(3, &tests);
    add_test(1, "Testing ensure_addres_in_higher_half", test_ensure_address_in_higher_half, tests);
    number_of_tests++;
    add_test(2, "Testing is address higher_half", test_is_address_higher_half, tests);
    number_of_tests++;
    add_test(3, "Testing test_vm_parse_flags", test_vm_parse_flags, tests);
    number_of_tests++;
}

void test_ensure_address_in_higher_half(unsigned int id) {
    //pretty_assert_stat(last, positions[0], ==, tests[id].stats, "(test_get_mountpoint_id): Testing path /home/dreamos82", has_pipe);
    uint64_t test_address = 0x100000;
    test_address = ensure_address_in_higher_half(test_address, 3);
    pretty_assert_stat(0, test_address, ==, tests[id].stats, "Should return (0) for type not recognized and address 0x100000", has_pipe);
    test_address = ensure_address_in_higher_half(0x100000, VM_TYPE_MMIO);
    pretty_assert_stat(0xffff800000300000, test_address, ==, tests[id].stats, "Testing for type MMIO and address 0x100000", has_pipe);
    test_address = ensure_address_in_higher_half(0x100000, VM_TYPE_MEMORY);    
    pretty_assert_stat(0xffff800280300000, test_address, ==, tests[id].stats, "Testing for type MEMORY and address 0x100000", has_pipe);    
    test_address = ensure_address_in_higher_half(0xffff800280300000, VM_TYPE_MEMORY);    
    pretty_assert_stat(0xffff800280300000, test_address, ==, tests[id].stats, "Testing for address: 0xffff800280300000 and type MEMORY", has_pipe);
}


void test_is_address_higher_half(unsigned int id) {
    bool is_hh = is_address_higher_half(0x100000);
    pretty_assert_stat(false, is_hh, ==, tests[id].stats, "Test with 0x100000 (lower half address)", has_pipe);    
    is_hh = is_address_higher_half(0xffff100000);
    pretty_assert_stat(false, is_hh, ==, tests[id].stats, "Test with 0xffff100000 (lower half address)", is_hh);
    is_hh = is_address_higher_half(0xFFFF800000000000);
    pretty_assert_stat(true, is_hh, ==, tests[id].stats, "Testing with 0xFFFF800000000000 (higher half address)", is_hh);
}

void test_vm_parse_flags(unsigned int id) {
     pretty_assert_stat(0, vm_parse_flags(VMM_FLAGS_ADDRESS_ONLY), ==, tests[id].stats, "Test with flags VMM_FLAGS_ADDRESS_ONLY", has_pipe);
     pretty_assert_stat(3, vm_parse_flags(VMM_FLAGS_ADDRESS_ONLY | VMM_FLAGS_PRESENT | VMM_FLAGS_WRITE_ENABLE), ==, tests[id].stats, "Test with VMM_FLAGS_ADDRESS_ONLY, PRESENT, WRITE_ENABLE", has_pipe);
     pretty_assert_stat(2, vm_parse_flags( VMM_FLAGS_WRITE_ENABLE), ==, tests[id].stats, "Test with VMM_FLAGS_WRITE_ENABLE", has_pipe);
     //pretty_assert_stat(3, vm_parse_flags(VMM_FLAGS_ADDRESS_ONLY | VMM_FLAGS_PRESENT | VMM_FLAGS_WRITE_ENABLE), ==, tests[id].stats, has_pipe);     
}
