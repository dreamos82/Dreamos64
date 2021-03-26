#include <vm.h>
#include <video.h>

void page_fault_handle(uint64_t error_code){
    _printStr("Welcome to #PF world - Not ready yet... \n");
    char number[10];
    uint64_t cr2_content = 0;
    
    asm ("mov %%cr2, %0" : "=r" (cr2_content) );
    _getHexString(number, error_code);
    _printStr(number);
    _printNewLine();
    _getHexString(number, cr2_content);
    _printStr(number);
    _printNewLine();
}
