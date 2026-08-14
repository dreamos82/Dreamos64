#include <assert.h>
#include <elf.h>
#include <stdio.h>
#include <stdlib.h>
#include <test_common.h>
#include <test_stats.h>
#include <unistd.h>
#include <utils.h>

void test_elf(unsigned int id);
void prepare_tests();
//void test_octodec();

Elf64_Ehdr elf_example;

bool has_pipe = false;
test_runner_t *tests;
unsigned int number_of_tests =0;

int main(int argc, char **argv) {
    int pipe_fd = -1;
    if (argc > 1) {
            //pipe_fd = atoi(argv[1]);
            has_pipe = true;
            pipe_fd = atoi(argv[1]);
            printf("Found pipe id: %d\n", pipe_fd);
    }
    prepare_tests();
    tests[0].handler(0);
    if ( has_pipe ) {
        write(pipe_fd, &number_of_tests, sizeof(number_of_tests));
        send_stats(pipe_fd, tests[0].stats);
        close(pipe_fd);
    }
    printf("\n");
}

void prepare_tests() {
    printf("Testing ELF functions  -\n");
    printf("===============================\n\n");
    test_init(2, &tests);
    add_test(1, "Testing ELF Validation", test_elf, tests);
    number_of_tests++;
}


void test_elf(unsigned int id){
    printf("%d) %s:\n", tests[id].stats.module_id, tests[id].stats.module_title);
    elf_example.e_ident[0] = 0x7f;
    elf_example.e_ident[1] = 'E';
    elf_example.e_ident[2] = 'L';
    elf_example.e_ident[3] = 'F';
    //pretty_assert(1, validate_elf_magic_number(&elf_example), ==, "Testing validate_elf_magic_number");
    pretty_assert_stat(1, validate_elf_magic_number(&elf_example), ==, tests[id].stats, "Testing validate_elf_magic_number", has_pipe);
    elf_example.e_ident[1] = 'O';
    //pretty_assert(0, validate_elf_magic_number(&elf_example), ==, "Testing validate_elf_magic_number");
    pretty_assert_stat(0, validate_elf_magic_number(&elf_example), ==, tests[id].stats, "Testing validate_elf_magic_number", has_pipe);
    elf_example.e_ident[0] = 'E';
    elf_example.e_ident[1] = 'E';
    //pretty_assert(0, validate_elf_magic_number(&elf_example), ==, "Testing validate_elf_magic_number");
    pretty_assert_stat(0, validate_elf_magic_number(&elf_example), ==, tests[id].stats, "Testing validate_elf_magic_number", has_pipe);
}


