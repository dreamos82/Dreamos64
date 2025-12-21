#include <assert.h>
#include <elf.h>
#include <stdio.h>
#include <test_common.h>
#include <utils.h>

void test_elf();
void test_octodec();

Elf64_Ehdr elf_example;
int main() {
    printf("Testing ELF functions  -\n");
    printf("===============================\n\n");
    test_elf();
    printf("\n");
}

void test_elf(){
    printf("Testing ELF Validation");
    elf_example.e_ident[0] = 0x7f;
    elf_example.e_ident[1] = 'E';
    elf_example.e_ident[2] = 'L';
    elf_example.e_ident[3] = 'F';
    pretty_assert(1, validate_elf_magic_number(&elf_example), ==, "Testing validate_elf_magic_number");
    elf_example.e_ident[1] = 'O';
    pretty_assert(0, validate_elf_magic_number(&elf_example), ==, "Testing validate_elf_magic_number");
    elf_example.e_ident[0] = 'E';
    elf_example.e_ident[1] = 'E';
    pretty_assert(0, validate_elf_magic_number(&elf_example), ==, "Testing validate_elf_magic_number");
}


