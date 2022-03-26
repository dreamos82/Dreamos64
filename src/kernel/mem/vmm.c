#include <bitmap.h>
#include <vmm.h>
#include <vm.h>
#include <video.h>
#include <pmm.h>
#include <stdio.h>

extern uint64_t p4_table[];
extern uint64_t p3_table_hh[];
extern uint64_t p2_table[];
extern uint64_t pt_tables[];

uint8_t is_phyisical_address_mapped(uint64_t physical_address, uint64_t virtual_address) {
    uint16_t pml4_e = PML4_ENTRY((uint64_t) virtual_address); 
    uint64_t *pml4_table = (uint64_t) (SIGN_EXTENSION | ENTRIES_TO_ADDRESS(510l, 510l, 510l, 510l));
    if ( !pml4_table[pml4_e] & PRESENT_BIT ) {
        return PHYS_ADDRESS_NOT_MAPPED;
    }

    uint16_t pdpr_e = PDPR_ENTRY((uint64_t) virtual_address);
    uint64_t *pdpr_table = (uint64_t) (SIGN_EXTENSION | ENTRIES_TO_ADDRESS(510l, 510l, 510l, (uint64_t)  pml4_e)); 
    if ( !pdpr_table[pdpr_e] & PRESENT_BIT) {
        return PHYS_ADDRESS_NOT_MAPPED;
    }

    uint16_t pd_e = PD_ENTRY((uint64_t) virtual_address);
    uint64_t *pd_table = (uint64_t) (SIGN_EXTENSION | ENTRIES_TO_ADDRESS(510l, 510l, pml4_e, (uint64_t)  pdpr_e));
    if ( !pd_table[pd_e] & PRESENT_BIT  ) {
        return PHYS_ADDRESS_NOT_MAPPED;
    }
#if SMALL_PAGES == 0    
    else {
        if (ALIGN_PHYSADDRESS(pd_table[pd_e]) == ALIGN_PHYSADDRESS(physical_address)) {
            return PHYS_ADDRESS_MAPPED;
        } else {
            return PHYS_ADDRESS_MISMATCH;
        }
    }
#endif

#if SMALL_PAGES == 1
    uint16_t pt_e = PT_ENTRY((uint64_t) virtual_address);
    uint64_t *pt_table = (uint64_t) (SIGN_EXTENSION | ENTRIES_TO_ADDRESS(510l, (uint64_t)  pml4_e, (uint64_t)  pdpr_e, (uint64_t)  pd_e));
    if ( !pt_table[pt_e] & PRESENT_BIT ) {
        return PHYS_ADDRESS_NOT_MAPPED;
    } else {
        if (ALIGN_PHYSADDRESS(pt_table[pt_e]) == ALIGN_PHYSADDRESS(physical_address)) {
            return PHYS_ADDRESS_MAPPED;
        } else {
            return PHYS_ADDRESS_MISMATCH;
        }
    }
#endif
    return 0;
}

int unmap_vaddress(void *address){
	uint16_t pml4_e = PML4_ENTRY((uint64_t) address);
	uint64_t *pml4_table = (uint64_t *) (SIGN_EXTENSION | ENTRIES_TO_ADDRESS(510l,510l,510l,510l));
	if(!(pml4_table[pml4_e] &0b1)){
		return -1;
	}
    uint16_t pdpr_e = PDPR_ENTRY((uint64_t) address);
	uint64_t *pdpr_table = (uint64_t *) (SIGN_EXTENSION | ENTRIES_TO_ADDRESS(510l,510l,510l, (uint64_t) pml4_e));
	if(!(pdpr_table[pdpr_e] & 0b1)){
		return -1;
	}
	uint64_t *pd_table = (uint64_t *) (SIGN_EXTENSION | ENTRIES_TO_ADDRESS(510l,510l, (uint64_t) pml4_e, (uint64_t)  pdpr_e));
    uint16_t pd_e = PD_ENTRY((uint64_t) address);
	if(!(pd_table[pd_e] & 0b01)){
		return -1;
	}
	
	#if SMALL_PAGES == 0
	printf("Freeing page\n");
	pd_table[pd_e] = 0x0l;
	invalidate_page_table(pd_table);
	#elif SMALL_PAGES == 1
	uint64_t *pt_table = SIGN_EXTENSION | ENTRIES_TO_ADDRESS(510l, (uint64_t)  pml4_e, (uint64_t)  pdpr_e, (uint64_t)  pd_e);
    uint16_t pt_e = PT_ENTRY((uint64_t) address);

	if(!(pt_table[pt_e] & 0b1)) {
		return -1;
	} 
	pt_table[pt_e] = 0x0l;
	invalidate_page_table(address);
	#endif

	return 0;
}


void *map_phys_to_virt_addr(void* physical_address, void* address, unsigned int flags){
    uint16_t pml4_e = PML4_ENTRY((uint64_t) address);
    uint64_t *pml4_table = (uint64_t *) (SIGN_EXTENSION | ENTRIES_TO_ADDRESS(510l,510l,510l,510l));
    // If the pml4_e item in the pml4 table is not present, we need to create a new one.
    // Every entry in pml4 table points to a pdpr table
    if( !(pml4_table[pml4_e] & 0b1) ) {
        uint64_t *new_table = pmm_alloc_frame();
        pml4_table[pml4_e] = (uint64_t) new_table | WRITE_BIT | PRESENT_BIT;
        clean_new_table(new_table);
    }

    uint16_t pdpr_e = PDPR_ENTRY((uint64_t) address);
    uint64_t *pdpr_table = (uint64_t *) (SIGN_EXTENSION | ENTRIES_TO_ADDRESS(510l,510l,510l, (uint64_t) pml4_e));

    // If the pdpr_e item in the pdpr table is not present, we need to create a new one.
    // Every entry in pdpr table points to a pdpr table
    if( !(pdpr_table[pdpr_e] & 0b1) ) {
        uint64_t *new_table = pmm_alloc_frame();
        pdpr_table[pdpr_e] = (uint64_t) new_table | WRITE_BIT | PRESENT_BIT;
        clean_new_table(new_table);
    }

    uint64_t *pd_table = (uint64_t *) (SIGN_EXTENSION | ENTRIES_TO_ADDRESS(510l,510l, (uint64_t) pml4_e, (uint64_t) pdpr_e));
    uint16_t pd_e = PD_ENTRY((uint64_t) address);

    // If the pd_e item in the pd table is not present, we need to create a new one.
    // Every entry in pdpr table points to a page table if using 4k pages, or to a 2mb memory area if using 2mb pages
    if( !(pd_table[pd_e] & 0b01) ) {
#if SMALL_PAGES == 1
        uint64_t *new_table = pmm_alloc_frame();
        pd_table[pd_e] = (uint64_t) new_table | WRITE_BIT | PRESENT_BIT;
        clean_new_table(new_table);
#elif SMALL_PAGES == 0
        pd_table[pd_e] = (uint64_t) (physical_address) | WRITE_BIT | PRESENT_BIT | HUGEPAGE_BIT;
#endif
    }

#if SMALL_PAGES == 1
    uint64_t *pt_table = (uint64_t *) (SIGN_EXTENSION | ENTRIES_TO_ADDRESS(510l, (uint64_t) pml4_e, (uint64_t) pdpr_e, (uint64_t) pd_e));
    uint16_t pt_e = PT_ENTRY((uint64_t) address);

    // This case apply only for 4kb pages, if the pt_e entry is not present in the page table we need to allocate a new 4k page
    // Every entry in the page table is a 4kb page of physical memory
    if( !(pt_table[pt_e] & 0b1)) {
        pt_table[pt_e] = (uint64_t) physical_address | flags | WRITE_BIT | PRESENT_BIT;
    }
#endif
    return address;
}

void *map_vaddress(void *virtual_address, unsigned int flags){
    void *new_addr = pmm_alloc_frame();
    return map_phys_to_virt_addr(new_addr, virtual_address, flags);
}

void map_vaddress_range(void *virtual_address, unsigned int flags, size_t required_pages) {
    for(size_t i = 0; i < required_pages; i++) {
        map_vaddress(virtual_address + (i * PAGE_SIZE_IN_BYTES), flags);
    }
}



