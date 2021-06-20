#include <test_common.h>
#include <stdio.h>

void _printStringAndNumber(char *string, unsigned long number){
    printf("%s0x%X\n", string, number);
}

void _printStr(char *string){
    printf("%s", string);
}

void _printNewLine(){
    printf("\n");
}

