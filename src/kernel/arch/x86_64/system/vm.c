#include <vm.h>
#include <video.h>
#include <framebuffer.h>
#include <main.h>

extern uint32_t FRAMEBUFFER_MEMORY_SIZE;

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
    pd = PD_ENTRY(cr2_content); 
    pdpr = PDPR_ENTRY(cr2_content);
    pml4 = PML4_ENTRY(cr2_content);
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

void clean_new_table(uint64_t *table_to_clean){
    for(int i = 0; i < VM_PAGES_PER_TABLE; i++){
        table_to_clean[i] = 0x00l;
    }
}

void invalidate_page_table(uint64_t *table_address){
	asm volatile("invlpg (%0)"
    	:
    	: "r"((uint64_t)table_address)
    	: "memory");
}

uint64_t ensure_address_in_higher_half( uint64_t address ) {
    if ( address > _HIGHER_HALF_KERNEL_MEM_START ) {
        return address;
    }
    return address + _HIGHER_HALF_KERNEL_MEM_START;
}
