#include <bitmap.h>
#include <vmm.h>
#include <vm.h>
#include <video.h>
#include <pmm.h>
#include <logging.h>
#include <kernel.h>
#include <bitmap.h>
#include <util.h>

extern uint64_t p4_table[];
extern uint64_t p3_table_hh[];
extern uint64_t p2_table[];
extern uint64_t pt_tables[];

VmmContainer *vmm_container_root;
VmmContainer *vmm_cur_container;
VmmItem *vmm_head;

size_t vmm_items_per_page;
size_t vmm_cur_index;

//TODO find better naming and probably one of them is not needed
size_t start_of_vmm_area;
size_t next_available_address;
uint64_t end_of_vmm_space;
/**
 * When initialized the VM Manager should reserve a portion of the virtual memory space for itself.
 */
void vmm_init() {

    vmm_container_root = ((uint64_t) HIGHER_HALF_ADDRESS_OFFSET + VM_KERNEL_MEMORY_PADDING);
    end_of_vmm_space = (uint64_t) vmm_container_root + VMM_RESERVED_SPACE_SIZE;
    start_of_vmm_area = (size_t) vmm_container_root + VMM_RESERVED_SPACE_SIZE + VM_KERNEL_MEMORY_PADDING;
    next_available_address = start_of_vmm_area;
    vmm_items_per_page = (PAGE_SIZE_IN_BYTES / sizeof(VmmItem)) - 1;
    vmm_cur_index = 0;

    loglinef(Verbose, "(vmm_init) Vmm root comparison: (vmm_root) %x - %x (end_of_vmm_space)", vmm_container_root, end_of_vmm_space);
    //I need to compute the size of the VMM address space
    uint64_t vmm_root_phys = pmm_alloc_frame();
    if (vmm_root_phys == NULL) {
        loglinef(Verbose, "(vmm_init)  vmm_root_phys should not be null");
        return;
    }
    
    loglinef(Verbose, "(vmm_init) Got vmm_root_phys address: %x", vmm_root_phys); 
    // Mapping the phyiscal address for the vmm structures
    map_phys_to_virt_addr(vmm_root_phys, vmm_container_root, 0);    
    loglinef(Verbose, "(vmm_init) Testing the just mapped address");
    vmm_container_root->vmm_root[0].base = 0;
    vmm_container_root->vmm_root[0].size = 5;
    vmm_container_root->vmm_root[0].flags = 0;
    loglinef(Verbose, "(vmm_init) flags should be 0: %d size should be 5: %d", vmm_container_root->vmm_root[0].flags, vmm_container_root->vmm_root[0].size);
    loglinef(Verbose, "(vmm_init) where does the container  start? %x", &vmm_container_root);
    loglinef(Verbose, "(vmm_init) where does the next end? %x", &(vmm_container_root->next));
    loglinef(Verbose, "(vmm_init) start of vmm_area %x", start_of_vmm_area);
    vmm_container_root->next = NULL;
    vmm_cur_container = vmm_container_root;
    vmm_head = NULL;
}

void *vmm_alloc(size_t length, size_t flags) {

    //TODO When the space inside this page is finished we need to allocate a new page
    //     at vmm_cur_container + sizeof(VmmItem)
    if (length < 0) {
        return NULL;
    }

    if ( vmm_head + sizeof(VmmItem) + length > end_of_vmm_space ) {
        // This case should never Happen, since the VMM space size is much bigger than the maximum ram that can be installed on a pc
        return NULL;
    }
    if (vmm_cur_index >= vmm_items_per_page) {
        logline(Verbose, "(vmm_init) Max number of pages reached, expansion to be implemented");
    }

    // Now i need to align the requested length to a page
    size_t new_length = align_value_to_page(length);
    loglinef(Verbose, "(vmm_alloc) length: %d - aligned: %d", length, new_length);

    uintptr_t address_to_return = next_available_address;
    vmm_cur_container->vmm_root[vmm_cur_index].base = address_to_return;
    vmm_cur_container->vmm_root[vmm_cur_index].flags = flags;
    vmm_cur_container->vmm_root[vmm_cur_index].size = new_length;
    next_available_address += new_length;
    loglinef(Verbose, "(vmm_alloc) newly allocated item base: %x, next available address: %x", vmm_cur_container->vmm_root[vmm_cur_index].base, next_available_address);
    vmm_cur_index++;

    return (void *) address_to_return;
}

void vmm_free(void *address) {
    loglinef(Verbose, "(vmm_free) To Be implemented address provided is: 0x%x", address);
    // Need to compute:
    // Page directories/table entries for the address
    // Search the base address inside the vmm objects arrays
    // Remove that object
    // Mark the entry as not present at least if it is mapped
    return;
}

uint8_t is_phyisical_address_mapped(uint64_t physical_address, uint64_t virtual_address) {
    uint16_t pml4_e = PML4_ENTRY((uint64_t) virtual_address); 
    uint64_t *pml4_table = (uint64_t *) (SIGN_EXTENSION | ENTRIES_TO_ADDRESS(510l, 510l, 510l, 510l));
    if ( !pml4_table[pml4_e] & PRESENT_BIT ) {
        return PHYS_ADDRESS_NOT_MAPPED;
    }

    uint16_t pdpr_e = PDPR_ENTRY((uint64_t) virtual_address);
    uint64_t *pdpr_table = (uint64_t *) (SIGN_EXTENSION | ENTRIES_TO_ADDRESS(510l, 510l, 510l, (uint64_t)  pml4_e)); 
    if ( !pdpr_table[pdpr_e] & PRESENT_BIT) {
        return PHYS_ADDRESS_NOT_MAPPED;
    }

    uint16_t pd_e = PD_ENTRY((uint64_t) virtual_address);
    uint64_t *pd_table = (uint64_t*) (SIGN_EXTENSION | ENTRIES_TO_ADDRESS(510l, 510l, pml4_e, (uint64_t)  pdpr_e));
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

void identity_map_phys_address(void *physical_address, unsigned int flags) {
    map_phys_to_virt_addr(physical_address, physical_address, flags);
}

/**
 * This function map a phyisical address into a virtual one. Both of them needs to already defined. 
 *
 *
 * @param physical_address the physical address we want to map
 * @param address the virtual address being mapped
 * @param flags the flags for the mapped page.
 * @return address the virtual address specified in input, or NULL in case of error.
 */
void *map_phys_to_virt_addr(void* physical_address, void* address, unsigned int flags){
    uint16_t pml4_e = PML4_ENTRY((uint64_t) address);
    uint64_t *pml4_table = (uint64_t *) (SIGN_EXTENSION | ENTRIES_TO_ADDRESS(510l,510l,510l,510l));
    
    uint16_t pdpr_e = PDPR_ENTRY((uint64_t) address);
    uint64_t *pdpr_table = (uint64_t *) (SIGN_EXTENSION | ENTRIES_TO_ADDRESS(510l,510l,510l, (uint64_t) pml4_e));

    uint64_t *pd_table = (uint64_t *) (SIGN_EXTENSION | ENTRIES_TO_ADDRESS(510l,510l, (uint64_t) pml4_e, (uint64_t) pdpr_e));
    uint16_t pd_e = PD_ENTRY((uint64_t) address);

    //loglinef(Verbose, "(map_phys_to_virt_addr) Pml4: %u - pdpr: %u - pd: %u", pml4_e, pdpr_e, pd_e);
    uint8_t user_mode_status = 0;
    
    #if SMALL_PAGES == 1

    uint64_t *pt_table = (uint64_t *) (SIGN_EXTENSION | ENTRIES_TO_ADDRESS(510l, (uint64_t) pml4_e, (uint64_t) pdpr_e, (uint64_t) pd_e));
    uint16_t pt_e = PT_ENTRY((uint64_t) address);

    #endif

    if ( !is_address_higher_half((uint64_t) address) ) {
        flags = flags | USER_LEVEL;
        user_mode_status = USER_LEVEL;
    }
    // If the pml4_e item in the pml4 table is not present, we need to create a new one.
    // Every entry in pml4 table points to a pdpr table
    if( !(pml4_table[pml4_e] & 0b1) ) {
        uint64_t *new_table = pmm_alloc_frame();
        pml4_table[pml4_e] = (uint64_t) new_table | user_mode_status | WRITE_BIT | PRESENT_BIT;
        clean_new_table(pdpr_table);
    }


    // If the pdpr_e item in the pdpr table is not present, we need to create a new one.
    // Every entry in pdpr table points to a pdpr table
    if( !(pdpr_table[pdpr_e] & 0b1) ) {
        uint64_t *new_table = pmm_alloc_frame();
        pdpr_table[pdpr_e] = (uint64_t) new_table | user_mode_status | WRITE_BIT | PRESENT_BIT;
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
        pd_table[pd_e] = (uint64_t) (physical_address) | WRITE_BIT | PRESENT_BIT | HUGEPAGE_BIT | flags;
#endif
    }

#if SMALL_PAGES == 1

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

uint8_t check_virt_address_status(uint64_t virtual_address) {
    return VIRT_ADDRESS_NOT_PRESENT;
}

