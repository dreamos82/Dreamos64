#include <test_common.h>
#include <stdio.h>
#include <bitmap.h>

void _printStringAndNumber(char *string, unsigned long number){
    printf("%s0x%X\n", string, number);
}

void _printStr(char *string){
    printf("%s", string);
}

void _printNewLine(){
    printf("\n");
}

uint32_t _compute_kernel_entries(){
    uint32_t kernel_entries = 0x1190AC / PAGE_SIZE_IN_BYTES;
    uint32_t kernel_mod_entries = 0x1190AC % PAGE_SIZE_IN_BYTES;
    printf("kernel_entries: 0x%X\n", kernel_entries);
    printf("kernel_mod_entries: 0x%X\n", kernel_mod_entries);
    if (  kernel_mod_entries != 0){
        return kernel_entries + 2;
    } 
    return kernel_entries + 1;
   
}

