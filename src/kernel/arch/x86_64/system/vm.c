#include <vm.h>
#include <video.h>
#include <framebuffer.h>
#include <main.h>
#include <logging.h>

extern uint32_t FRAMEBUFFER_MEMORY_SIZE;

void page_fault_handler(uint64_t error_code) {
    // TODO: Add ptable info when using 4k pages
    logline(Verbose, "Welcome to #PF world - Not ready yet... ");
    uint64_t cr2_content = 0;
    uint64_t pd;
    uint64_t pdpr;
    uint64_t pml4;  
    asm ("mov %%cr2, %0" : "=r" (cr2_content) );
    loglinef(Verbose, "-- Error code value: %d", error_code);
    loglinef(Verbose, "--  Faulting address: 0x%X", cr2_content);
    cr2_content = cr2_content & VM_OFFSET_MASK;
    loglinef(Verbose, "-- Address prepared for PD/PT extraction: %x", cr2_content);
    pd = PD_ENTRY(cr2_content); 
    pdpr = PDPR_ENTRY(cr2_content);
    pml4 = PML4_ENTRY(cr2_content);
    loglinef(Verbose, "Error flags: FETCH(%d) - RSVD(%d) - ACCESS(%d) - WRITE(%d) - PRESENT(%d)", \
            error_code&FETCH_VIOLATION, \
            error_code&RESERVED_VIOLATION, \
            error_code&ACCESS_VIOLATION, \
            error_code&WRITE_VIOLATION, \
            error_code&PRESENT_VIOLATION);
    uint64_t *pd_table = (uint64_t *) (SIGN_EXTENSION | ENTRIES_TO_ADDRESS(510l,510l, (uint64_t) pml4, (uint64_t) pdpr));
    uint64_t *pdpr_table = (uint64_t *) (SIGN_EXTENSION | ENTRIES_TO_ADDRESS(510l,510l, 510l, (uint64_t) pml4));
    uint64_t *pml4_table = (uint64_t *) (SIGN_EXTENSION | ENTRIES_TO_ADDRESS(510l,510l, 510l, 510l));
    loglinef(Verbose, "Entries: pd: 0x%X - pdpr: 0x%X - PML4 0x%X", pd, pdpr, pml4);
    loglinef(Verbose, "Entries: pd[0x%x]: 0x%X", pd, pd_table[pd]);
    loglinef(Verbose, "Entries: pdpr[0x%x]: 0x%X", pdpr, pdpr_table[pdpr]);
    loglinef(Verbose, "Entries: pml4[0x%x]: 0x%X", pml4, pml4_table[pml4]);
    asm("hlt");
}

void clean_new_table( uint64_t *table_to_clean ) {
    for(int i = 0; i < VM_PAGES_PER_TABLE; i++){
        table_to_clean[i] = 0x00l;
    }
}

void load_cr3( void* cr3_value ) {
    //This function is used only when the kernel needs to load a new pml4 table (paging root for x86_64)
    //I should add support for the flags
    //invalidate_page_table((uint64_t) cr3_value);
    //loglinef(Verbose, "(load_cr3) Loading pml4 table at address: %u", cr3_value);
    asm volatile("mov %0, %%cr3" :: "r"((uint64_t)cr3_value) : "memory");
}

void invalidate_page_table( uint64_t *table_address ) {
	asm volatile("invlpg (%0)"
    	:
    	: "r"((uint64_t)table_address)
    	: "memory");
}


/**
 * This function given a vmm flags variable, return the architecture dependent value
 *
 *
 * @param flags vmm flags
 * @return architecture independat flags 
 */
size_t vm_parse_flags( paging_flags_t flags) {
    // This function return the vmm_alloc flags parsed to be used with the x86_64 architectture page tables.
    flags = flags & ~(1 << 7); 
    return flags;
}

/**
 * This function given an address if it is not in the higher half, it return the same address + HIGHER_HALF_ADDRESS_OFFSET already defined. 
 *
 *
 * @param address the physical address we want to map
 * @return virtuial address in the higher half
 */
uint64_t ensure_address_in_higher_half( uint64_t address ) {
    if ( address > HIGHER_HALF_ADDRESS_OFFSET ) {
        return address;
    }
    return address + HIGHER_HALF_ADDRESS_OFFSET;
}

bool is_address_higher_half(uint64_t address) {
    if ( address & (1l << 62) ) {
        return true;
    }
    return false;
}
