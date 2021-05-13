#include <vm.h>
#include <video.h>

void page_fault_handler(uint64_t error_code){
    _printStr("Welcome to #PF world - Not ready yet... \n");
    uint64_t cr2_content = 0;
    uint64_t pd;
    uint64_t pdpr;
    uint64_t pml4;  
    asm ("mov %%cr2, %0" : "=r" (cr2_content) );
    _printStringAndNumber("-- Error code value: ", error_code);
    _printStringAndNumber("--  Faulting address: ", cr2_content);
    cr2_content = cr2_content & VM_OFFSET_MASK;
    _printStringAndNumber("-- Address prepared for PD/PT extraction: ", cr2_content);
    _printStringAndNumber("-- VM_OFFSET_MASK: ", VM_OFFSET_MASK);
    pd = (cr2_content>>21) & VM_AND_MASK; 
    pdpr = (cr2_content>>30) & VM_AND_MASK;
    pml4 = (cr2_content>>39) & VM_AND_MASK; 
    _printStringAndNumber("pd: ", pd);
    _printStringAndNumber("pdpr: ", pdpr);
    _printStringAndNumber("pml4: ", pml4);
    asm("hlt");

}


void initialize_vm(){
    //This function will map essential part of the memory (FB, ACPI stuff)
#ifdef USE_FRAMEBUFFER

#endif
}
