#include <logging.h>
#include <pmm.h>
#include <vm.h>
#include <vmm.h>

/**
 * This function map a phyisical address into a virtual one. Both of them needs to already defined.
 *
 *
 * @param physical_address the physical address we want to map
 * @param address the virtual address being mapped
 * @param flags the flags for the mapped page.
 * @param pml4_root the pointer to the pml4 table, if null it will use the kernel default pml4 table.
 * @return address the virtual address specified in input, or NULL in case of error.
 */
void *map_phys_to_virt_addr(void* physical_address, void* address, size_t flags, uint64_t *pml4_root){
    uint16_t pml4_e = PML4_ENTRY((uint64_t) address);
    uint16_t pdpr_e = PDPR_ENTRY((uint64_t) address);
    uint16_t pd_e = PD_ENTRY((uint64_t) address);

    if (pml4_root != NULL) {
        loglinef(Verbose, "(%s): Work in progress...", __FUNCTION__, pml4_root[510]);
    }

    uint64_t *pml4_table = (uint64_t *) (SIGN_EXTENSION | ENTRIES_TO_ADDRESS(510l,510l,510l,510l));
    uint64_t *pdpr_table = (uint64_t *) (SIGN_EXTENSION | ENTRIES_TO_ADDRESS(510l,510l,510l, (uint64_t) pml4_e));
    uint64_t *pd_table = (uint64_t *) (SIGN_EXTENSION | ENTRIES_TO_ADDRESS(510l,510l, (uint64_t) pml4_e, (uint64_t) pdpr_e));

//    loglinef(Verbose, "(map_phys_to_virt_addr) Pml4: %u - pdpr: %u - pd: %u - flags: 0x%x to address: 0x%x", pml4_e, pdpr_e, pd_e, flags, address);
    uint8_t user_mode_status = 0;

    #if SMALL_PAGES == 1

    uint64_t *pt_table = (uint64_t *) (SIGN_EXTENSION | ENTRIES_TO_ADDRESS(510l, (uint64_t) pml4_e, (uint64_t) pdpr_e, (uint64_t) pd_e));
    uint16_t pt_e = PT_ENTRY((uint64_t) address);

    #endif

    if ( !is_address_higher_half((uint64_t) address) ) {
        flags = flags | VMM_FLAGS_USER_LEVEL;
        user_mode_status = VMM_FLAGS_USER_LEVEL;
    }

    // If the pml4_e item in the pml4 table is not present, we need to create a new one.
    // Every entry in pml4 table points to a pdpr table
    if( !(pml4_table[pml4_e] & 0b1) ) {
        uint64_t *new_table = pmm_alloc_frame();
        pml4_table[pml4_e] = (uint64_t) new_table | user_mode_status | WRITE_BIT | PRESENT_BIT;
//        loglinef(Verbose, "(%s): need to allocate pml4 for address: 0x%x - Entry value: 0x%x - phys_address: 0x%x", __FUNCTION__, (uint64_t) address, pml4_table[pml4_e], new_table);
        clean_new_table(pdpr_table);
    }



    // If the pdpr_e item in the pdpr table is not present, we need to create a new one.
    // Every entry in pdpr table points to a pdpr table
    if( !(pdpr_table[pdpr_e] & 0b1) ) {
        uint64_t *new_table = pmm_alloc_frame();
        pdpr_table[pdpr_e] = (uint64_t) new_table | user_mode_status | WRITE_BIT | PRESENT_BIT;
//        loglinef(Verbose, "(%s): PDPR entry value: 0x%x", __FUNCTION__, pdpr_table[pdpr_e]);
        clean_new_table(pd_table);
    }

    // If the pd_e item in the pd table is not present, we need to create a new one.
    // Every entry in pdpr table points to a page table if using 4k pages, or to a 2mb memory area if using 2mb pages
    if( !(pd_table[pd_e] & 0b01) ) {
#if SMALL_PAGES == 1
        uint64_t *new_table = pmm_alloc_frame();
        pd_table[pd_e] = (uint64_t) new_table | user_mode_status | WRITE_BIT | PRESENT_BIT;
        clean_new_table(pt_table);
#elif SMALL_PAGES == 0
        pd_table[pd_e] = (uint64_t) (physical_address) | HUGEPAGE_BIT | flags | user_mode_status;
//       loglinef(Verbose, "(%s): PD Flags: 0x%x entry value: 0x%x", __FUNCTION__, flags, pd_table[pd_e]);
#endif
    }

#if SMALL_PAGES == 1

    // This case apply only for 4kb pages, if the pt_e entry is not present in the page table we need to allocate a new 4k page
    // Every entry in the page table is a 4kb page of physical memory
    if( !(pt_table[pt_e] & 0b1)) {
        pt_table[pt_e] = (uint64_t) physical_address | flags;
    }
#endif
    return address;
}

void *map_vaddress(void *virtual_address, size_t flags, uint64_t *pml4_root){
    loglinef(Verbose, "(map_vaddress) address: 0x%x", virtual_address);
    void *new_addr = pmm_alloc_frame();
    return map_phys_to_virt_addr(new_addr, virtual_address, flags, pml4_root);
}

void map_vaddress_range(void *virtual_address, size_t flags, size_t required_pages, uint64_t *pml4_root) {
    for(size_t i = 0; i < required_pages; i++) {
        map_vaddress(virtual_address + (i * PAGE_SIZE_IN_BYTES), flags, NULL);
    }
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
	logline(Verbose, "Freeing page");
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

void identity_map_phys_address(void *physical_address, size_t flags) {
    map_phys_to_virt_addr(physical_address, physical_address, flags, NULL);
}
