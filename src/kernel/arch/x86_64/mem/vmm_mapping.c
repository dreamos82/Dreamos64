#include <hh_direct_map.h>
#include <kernel.h>
#include <logging.h>
#include <pmm.h>
#include <vm.h>
#include <vmm.h>
#include <vmm_mapping.h>

void *map_phys_to_virt_addr_hh(void* physical_address, void* address, size_t flags, uint64_t *pml4_root) {

    uint16_t pml4_e = PML4_ENTRY((uint64_t) address);
    uint16_t pdpr_e = PDPR_ENTRY((uint64_t) address);
    uint16_t pd_e = PD_ENTRY((uint64_t) address);

    //uint64_t *pml4_table = NULL;
    uint64_t *pdpr_root = NULL;
    uint64_t *pd_root = NULL;

#if SMALL_PAGES == 1
    uint64_t *pt_table = NULL;
    uint16_t pt_e = PT_ENTRY((uint64_t) address);
#endif

    uint8_t user_mode_status = 0;

    if ( !is_address_higher_half((uint64_t) address) ) {
        pretty_logf(Verbose, "address is in lower half: 0x%x", address);
        flags = flags | VMM_FLAGS_USER_LEVEL;
        user_mode_status = VMM_FLAGS_USER_LEVEL;
    }

    if (pml4_root == NULL) {
        pml4_root = kernel_settings.paging.hhdm_page_root_address;
    }

    if (pml4_root != NULL) {
        pretty_logf(Verbose, "Entries values pml4_e: 0x%d pdpr_e: 0x%d pd_e: 0x%d", pml4_e, pdpr_e, pd_e);
        pretty_logf(Verbose, "\taddress: 0x%x, phys_address: 0x%x", address, physical_address);
        pretty_logf(Verbose, "\tpdpr base_address: 0x%x", pml4_root[pml4_e] & VM_PAGE_TABLE_BASE_ADDRESS_MASK);

        if ( !(pml4_root[pml4_e] & 0b1) ) {
            pretty_logf(Verbose, " We should allocate a new table at pml4_e: %d", pml4_e);
            uint64_t *new_table = pmm_prepare_new_pagetable();
            pml4_root[pml4_e] = (uint64_t) new_table | user_mode_status | WRITE_BIT | PRESENT_BIT;
            uint64_t *new_table_hhdm = hhdm_get_variable((uintptr_t) new_table);
            clean_new_table(new_table_hhdm);
            pdpr_root = new_table_hhdm;
        } else {
            pdpr_root =  (uint64_t *) hhdm_get_variable((uintptr_t) pml4_root[pml4_e] & VM_PAGE_TABLE_BASE_ADDRESS_MASK);
        }

        if ( !(pdpr_root[pdpr_e] & 0b1) ) {
            pretty_logf(Verbose, " We should allocate a new table at pdpr_e: %d", pdpr_e);
            uint64_t *new_table = pmm_prepare_new_pagetable();
            pdpr_root[pdpr_e] = (uint64_t) new_table | user_mode_status | WRITE_BIT | PRESENT_BIT;
            uint64_t *new_table_hhdm = hhdm_get_variable((uintptr_t) new_table);
            clean_new_table(new_table_hhdm);
            pd_root = new_table_hhdm;
        } else {
            pd_root =  (uint64_t *) hhdm_get_variable((uintptr_t) pdpr_root[pdpr_e] & VM_PAGE_TABLE_BASE_ADDRESS_MASK);
        }

        if( !(pd_root[pd_e] & 0b1) ) {

#if SMALL_PAGES == 1
            uint64_t *new_table = pmm_prepare_new_pagetable();
            pd_root[pd_e] = (uint64_t) new_table | user_mode_status | WRITE_BIT | PRESENT_BIT;
            uint64_t *new_table_hhdm = hhdm_get_variable((uintptr_t) new_table);
            clean_new_table(new_table_hhdm);
            pt_table = new_table_hhdm;
        } else {
            pt_table = (uint64_t *) hhdm_get_variable((uintptr_t) pd_root[pd_e] & VM_PAGE_TABLE_BASE_ADDRESS_MASK);
        }

        // This case apply only for 4kb pages, if the pt_e entry is not present in the page table we need to allocate a new 4k page
        // Every entry in the page table is a 4kb page of physical memory
        if( !(pt_table[pt_e] & 0b1)) {
            pt_table[pt_e] = (uint64_t) physical_address | flags;
            return address;
        }
#elif SMALL_PAGES == 0
            pd_root[pd_e] = (uint64_t) (physical_address) | HUGEPAGE_BIT | flags | user_mode_status;
            pretty_logf(Verbose, " PD Flags: 0x%x entry value pd_root[0x%x]: 0x%x - address: 0x%x", flags, pd_e, pd_root[pd_e], address);
            return address;
        }
#endif
    }

    //pml4_table = pml4_root
    return NULL;
}


/**
 * This function map a phyisical address into a virtual one. Both of them needs to already defined.
 *
 *
 * @param physical_address the physical address we want to map
 * @param address the virtual address being mapped
 * @param flags the flags for the mapped page.
 * @param pml4_root the pointer to the pml4 table, It should be the hhdm address. if null it will use the kernel default pml4 table.
 * @return address the virtual address specified in input, or NULL in case of error.
 */
void *map_phys_to_virt_addr(void* physical_address, void* address, size_t flags) {
    uint16_t pml4_e = PML4_ENTRY((uint64_t) address);
    uint16_t pdpr_e = PDPR_ENTRY((uint64_t) address);
    uint16_t pd_e = PD_ENTRY((uint64_t) address);

    uint64_t *pml4_table = (uint64_t *) (SIGN_EXTENSION | ENTRIES_TO_ADDRESS(510l,510l,510l,510l));

    uint8_t user_mode_status = 0;

    if ( !is_address_higher_half((uint64_t) address) ) {
        flags = flags | VMM_FLAGS_USER_LEVEL;
        user_mode_status = VMM_FLAGS_USER_LEVEL;
    }

    uint64_t *pdpr_table = (uint64_t *) (SIGN_EXTENSION | ENTRIES_TO_ADDRESS(510l,510l,510l, (uint64_t) pml4_e));
    uint64_t *pd_table = (uint64_t *) (SIGN_EXTENSION | ENTRIES_TO_ADDRESS(510l,510l, (uint64_t) pml4_e, (uint64_t) pdpr_e));
    //pretty_logf(Verbose, "(map_phys_to_virt_addr) Pml4: %u - pdpr: %u - pd: %u - flags: 0x%x to address: 0x%x", pml4_e, pdpr_e, pd_e, flags, address);

    #if SMALL_PAGES == 1

    uint64_t *pt_table = (uint64_t *) (SIGN_EXTENSION | ENTRIES_TO_ADDRESS(510l, (uint64_t) pml4_e, (uint64_t) pdpr_e, (uint64_t) pd_e));
    uint16_t pt_e = PT_ENTRY((uint64_t) address);

    #endif

    // If the pml4_e item in the pml4 table is not present, we need to create a new one.
    // Every entry in pml4 table points to a pdpr table
    if( !(pml4_table[pml4_e] & 0b1) ) {
        uint64_t *new_table = pmm_prepare_new_pagetable();;
        pml4_table[pml4_e] = (uint64_t) new_table | user_mode_status | WRITE_BIT | PRESENT_BIT;
       // pretty_logf(Verbose, " need to allocate pml4 for address: 0x%x - Entry value: 0x%x - phys_address: 0x%x", (uint64_t) address, pml4_table[pml4_e], new_table);
        clean_new_table(pdpr_table);
    }

    // If the pdpr_e item in the pdpr table is not present, we need to create a new one.
    // Every entry in pdpr table points to a pdpr table
    if( !(pdpr_table[pdpr_e] & 0b1) ) {
        uint64_t *new_table = pmm_prepare_new_pagetable();
        pdpr_table[pdpr_e] = (uint64_t) new_table | user_mode_status | WRITE_BIT | PRESENT_BIT;
        //pretty_logf(Verbose, " PDPR entry value: 0x%x", pdpr_table[pdpr_e]);
        clean_new_table(pd_table);
    }

    // If the pd_e item in the pd table is not present, we need to create a new one.
    // Every entry in pdpr table points to a page table if using 4k pages, or to a 2mb memory area if using 2mb pages
    if( !(pd_table[pd_e] & 0b01) ) {
#if SMALL_PAGES == 1
        uint64_t *new_table = pmm_prepare_new_pagetable();
        pd_table[pd_e] = (uint64_t) new_table | user_mode_status | WRITE_BIT | PRESENT_BIT;
        clean_new_table(pt_table);
#elif SMALL_PAGES == 0
        pd_table[pd_e] = (uint64_t) (physical_address) | HUGEPAGE_BIT | flags | user_mode_status;
//       pretty_logf(Verbose, " PD Flags: 0x%x entry value: 0x%x", flags, pd_table[pd_e]);
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
    pretty_logf(Verbose, "address: 0x%x", virtual_address);
    //TODO need to check if i can just use the phys alloc here
    void *new_addr = pmm_prepare_new_pagetable();
    return map_phys_to_virt_addr_hh(new_addr, virtual_address, flags, pml4_root);
}

void map_vaddress_range(void *virtual_address, size_t flags, size_t required_pages, uint64_t *pml4_root) {
    for(size_t i = 0; i < required_pages; i++) {
        map_vaddress(virtual_address + (i * PAGE_SIZE_IN_BYTES), flags, pml4_root);
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
	pretty_log(Verbose, "Freeing page");
	pd_table[pd_e] = 0x0l;
	invalidate_page_table(pd_table);
#elif SMALL_PAGES == 1
	uint64_t *pt_table = (uint64_t *) (SIGN_EXTENSION | ENTRIES_TO_ADDRESS(510l, (uint64_t)  pml4_e, (uint64_t)  pdpr_e, (uint64_t)  pd_e));
    uint16_t pt_e = PT_ENTRY((uint64_t) address);

	if(!(pt_table[pt_e] & 0b1)) {
		return -1;
	}
	pt_table[pt_e] = 0x0l;
	invalidate_page_table(address);
#endif

	return 0;
}

int unmap_vaddress_hh(void *address, uint64_t *pml4_root) {
    if ( pml4_root == NULL || address == NULL ) {
        return -1;
    }

    uint16_t pml4_e = PML4_ENTRY((uint64_t) address);
    uint64_t *pml4_table = pml4_root;
    pretty_logf(Verbose, " pml4_table[%d] = 0x%x", pml4_e, pml4_table[pml4_e]);
    if (!(pml4_table[pml4_e] & 0b01)) {
        return -1;
    }
    uint16_t pdpr_e = PDPR_ENTRY((uint64_t) address);
    uint64_t *pdpr_table =  (uint64_t *) hhdm_get_variable((uintptr_t) pml4_table[pml4_e] & VM_PAGE_TABLE_BASE_ADDRESS_MASK);
    pretty_logf(Verbose, " pdpr_table[%d] = 0x%x", pdpr_e, pdpr_table[pdpr_e]);
    if (!(pdpr_table[pdpr_e] & 0b01)) {
        return -1;
    }
    uint16_t pd_e = PD_ENTRY((uint64_t) address);
    uint64_t *pd_table = (uint64_t *) hhdm_get_variable((uintptr_t) pdpr_table[pdpr_e] & VM_PAGE_TABLE_BASE_ADDRESS_MASK);
    pretty_logf(Verbose, " pd_table[%d] = 0x%x", pd_e, pd_table[pd_e]);

    if (!(pd_table[pd_e] &0b01)) {
        return -1;
    }

#if SMALL_PAGES == 0
    pretty_logf(Verbose, " Unmapping address: 0x%x, pd_entry: %d", address, pd_e);
    pd_table[pd_e] = 0x0l;
    invalidate_page_table(address);
    return 0;
#elif SMALL_PAGES == 1
    uint64_t *pt_table = (uint64_t *) hhdm_get_variable((uintptr_t) pd_table[pd_e] & VM_PAGE_TABLE_BASE_ADDRESS_MASK);
    uint16_t pt_e = PT_ENTRY((uint64_t) address);
    pretty_logf(Verbose, " Unmapping address: 0x%x, pd_entry: %d", address, pt_e);
    if ( !(pt_table[pt_e] & 0b01) ) {
        return -1;
    }

    pt_table[pt_e] = 0x0l;
    invalidate_page_table(address);
    return 0;
#endif
}

//TODO This function is no longer used, it may be removed in the future
void identity_map_phys_address(void *physical_address, size_t flags) {
    map_phys_to_virt_addr(physical_address, physical_address, flags);
}

uint8_t is_phyisical_address_mapped(uintptr_t physical_address, uintptr_t virtual_address) {
    uint16_t pml4_e = PML4_ENTRY((uint64_t) virtual_address);
    uint64_t *pml4_table = (uint64_t *) (SIGN_EXTENSION | ENTRIES_TO_ADDRESS(510l, 510l, 510l, 510l));
    if (!(pml4_table[pml4_e] & PRESENT_BIT)) {
        return PHYS_ADDRESS_NOT_MAPPED;
    }

    uint16_t pdpr_e = PDPR_ENTRY((uint64_t) virtual_address);
    uint64_t *pdpr_table = (uint64_t *) (SIGN_EXTENSION | ENTRIES_TO_ADDRESS(510l, 510l, 510l, (uint64_t)  pml4_e));
    if (!(pdpr_table[pdpr_e] & PRESENT_BIT)) {
        return PHYS_ADDRESS_NOT_MAPPED;
    }

    uint16_t pd_e = PD_ENTRY((uint64_t) virtual_address);
    uint64_t *pd_table = (uint64_t*) (SIGN_EXTENSION | ENTRIES_TO_ADDRESS(510l, 510l, pml4_e, (uint64_t)  pdpr_e));
    if (!(pd_table[pd_e] & PRESENT_BIT)) {
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
    uint64_t *pt_table = (uint64_t *) (SIGN_EXTENSION | ENTRIES_TO_ADDRESS(510l, (uint64_t)  pml4_e, (uint64_t)  pdpr_e, (uint64_t)  pd_e));
    if ( !(pt_table[pt_e] & PRESENT_BIT )) {
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

/**
 * This function given a virtual address and the hhdm address its address space return the
 * hhdm kernel space equivalent of it.
 *
 *
 * @param virtual_address a virtual address in any virtual space
 * @param root_table_hhdm the hhdm address of the pml4 table containing the virtual address
 *
 * @return hhdm kernel space equivalent of virtual address.
 */
uintptr_t vm_copy_from_different_space(uintptr_t virtual_address, uint64_t *root_table_hhdm){
    /**
     * Steps
     * 1. I need to get PML$, PDPR, PD and PT (if 4k pages are used) entries value for virtual address
     * 2. Starting from pml4 get the next level table using the entries above.
     * 3. When arrived at last level (PD or PT depending on the page size) extract the physical address from that entry.
     * 4. Return the hhdm translation in kernel space of the physical address
     */
    uint16_t pml4_entry = PML4_ENTRY((uint64_t) virtual_address);
    uint16_t pdpr_entry = PDPR_ENTRY((uint64_t) virtual_address);
    uint16_t pd_entry = PD_ENTRY((uint64_t) virtual_address);
    pretty_logf(Verbose, "pml4_e: %d - pdpr_e: %d - pd_e: %d", pml4_entry, pdpr_entry, pd_entry);
#if SMALL_PAGES == 1
    uint16_t pt_entry = PT_ENTRY((uint64_t) virtual_address);
#endif

    if (!(root_table_hhdm[pml4_entry] & 0b1)) {
        pretty_logf(Error, "Cannot find a valid mapping for pml4_entry: 0x%d on memory space: 0x%x", pml4_entry, root_table_hhdm);
        return 0;
    }

    uint64_t *pdpr_addr = (uint64_t *) hhdm_get_variable((uintptr_t) root_table_hhdm[pml4_entry] & VM_PAGE_TABLE_BASE_ADDRESS_MASK);

    if (!(pdpr_addr[pdpr_entry] & 0b1)) {
        pretty_logf(Verbose, "pdpr addr: 0x%x", pdpr_addr[pd_entry]);
        return 0;
    }

    uint64_t *pd_addr = (uint64_t *) hhdm_get_variable((uintptr_t) pdpr_addr[pdpr_entry] & VM_PAGE_TABLE_BASE_ADDRESS_MASK);

    if (!(pd_addr[pd_entry] & 0b1)) {
        return 0;
    }

#if SMALL_PAGES==1

    uint64_t *pt_addr = (uint64_t *) hhdm_get_variable((uintptr_t) pd_addr[pt_entry] & VM_PAGE_TABLE_BASE_ADDRESS_MASK);
    uint64_t table_entry_value = (uint64_t) pt_addr[pt_entry];

#elif SMALL_PAGES==0
    uint64_t table_entry_value = (uint64_t) pd_addr[pd_entry];
#endif

    vm_walk_results walked_address = vm_walk_table((void *) virtual_address, (uint64_t*) root_table_hhdm);

    uintptr_t local_virt_address =  (uintptr_t) hhdm_get_variable((uintptr_t) table_entry_value & VM_PAGE_TABLE_BASE_ADDRESS_MASK);
    pretty_logf(Verbose, "cur_result: 0x%x - walk_result: 0x%x - Completed: %d - Level: %x", local_virt_address, walked_address.pte, walked_address.completed, walked_address.level);
    pretty_logf(Verbose, "table entry value: 0x%x - phys_address: 0x%x", table_entry_value, table_entry_value & VM_PAGE_TABLE_BASE_ADDRESS_MASK);

    return local_virt_address;
}

vm_walk_results vm_walk_table(void* virtual_address, uint64_t *root_table_hhdm) {
    vm_walk_results walk_results;
    uint16_t pml4_entry = PML4_ENTRY((uint64_t) virtual_address);
    uint16_t pdpr_entry = PDPR_ENTRY((uint64_t) virtual_address);
    uint16_t pd_entry = PD_ENTRY((uint64_t) virtual_address);
    walk_results.level = 0;
    walk_results.pte = NULL;
    walk_results.completed = false;
    pretty_logf(Verbose, "pml4_e: %d - pdpr_e: %d - pd_e: %d", pml4_entry, pdpr_entry, pd_entry);
#if SMALL_PAGES == 1
    uint16_t pt_entry = PT_ENTRY((uint64_t) virtual_address);
#endif

    if (!(root_table_hhdm[pml4_entry] & PRESENT_BIT)) {
        pretty_logf(Error, "Cannot find a valid mapping for pml4_entry: 0x%d on memory space: 0x%x", pml4_entry, root_table_hhdm);
        return walk_results;
    }

    walk_results.level++;

    uint64_t *pdpr_addr = (uint64_t *) hhdm_get_variable((uintptr_t) root_table_hhdm[pml4_entry] & VM_PAGE_TABLE_BASE_ADDRESS_MASK);

    if (!(pdpr_addr[pdpr_entry] & PRESENT_BIT)) {
        pretty_logf(Error, "PDPR_ENTRY %d present bit not set. Address: 0x%x", pdpr_entry, pdpr_addr);
        return walk_results;
    }

    walk_results.level++;

    uint64_t *pd_addr = (uint64_t *) hhdm_get_variable((uintptr_t) pdpr_addr[pdpr_entry] & VM_PAGE_TABLE_BASE_ADDRESS_MASK);

    if (!(pd_addr[pd_entry] & PRESENT_BIT)) {
        pretty_logf(Error, "PD_ENTRY %d present bit not set. Address: 0x%x", pd_entry, pd_addr);
        return walk_results;
    }

#if SMALL_PAGES == 0
    walk_results.completed = true;
    uint64_t table_entry_value = (uint64_t) pd_addr[pd_entry];
#elif SMALL_PAGES == 1
    walk_results.level++;


    uint64_t *pt_addr = (uint64_t *) hhdm_get_variable((uintptr_t) pd_addr[pt_entry] & VM_PAGE_TABLE_BASE_ADDRESS_MASK);
    uint64_t table_entry_value = (uint64_t) pt_addr[pt_entry];

    if(!(table_entry_value & PRESENT_BIT)) {
        pretty_logf(Error, "PT_ENTRY %d present bit not set. Address: 0x%x", pt_entry, table_entry_value);
        return walk_results;
    }
#endif

    uintptr_t local_virt_address =  (uintptr_t) hhdm_get_variable((uintptr_t) table_entry_value & VM_PAGE_TABLE_BASE_ADDRESS_MASK);
    pretty_logf(Verbose, "table entry value: 0x%x - phys_address: 0x%x", table_entry_value, table_entry_value & VM_PAGE_TABLE_BASE_ADDRESS_MASK);

    walk_results.pte = (uint64_t *) local_virt_address;
    return walk_results;
}

