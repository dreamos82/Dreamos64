#include <vm.h>
#include <video.h>

void page_fault_handle(uint64_t error_code){
    _printStr("Welcome to #PF world - Not ready yet... \n");
    uint64_t cr2_content = 0;
    
    asm ("mov %%cr2, %0" : "=r" (cr2_content) );
    _printStringAndNumber("-- Error code value: ", error_code);
    _printStringAndNumber("--  Faulting address: ", cr2_content);
    asm("hlt");
}
