#include <bitmap.h>
#include <kernel.h>
#include <logging.h>
#include <main.h>
#include <pmm.h>
#include <video.h>
#include <vm.h>
#include <vmm.h>
#include <vmm_mapping.h>
#include <vmm_util.h>


extern uint64_t p4_table[];
extern uint64_t p3_table_hh[];
extern uint64_t p2_table[];
extern uint64_t pt_tables[];

VmmContainer *vmm_container_root;
VmmContainer *vmm_cur_container;

size_t vmm_items_per_page;
size_t vmm_cur_index;

//TODO find better naming and probably one of them is not needed
size_t start_of_vmm_space;
size_t next_available_address;
uint64_t end_of_vmm_data;
VmmInfo vmm_kernel;

/**
 * When initialized the VM Manager should reserve a portion of the virtual memory space for itself.
 */
void vmm_init(vmm_level_t vmm_level, VmmInfo *vmm_info) {

    uint64_t *root_table_hh;

    if (vmm_info == NULL) {
        loglinef(Verbose, "(%s): kernel vmm initialization", __FUNCTION__);
        vmm_info = &vmm_kernel;
        root_table_hh = kernel_settings.paging.hhdm_page_root_address;
    } else {
        loglinef(Verbose, "(%s): task vmm initialization: root_table_hhdm: 0x%x", __FUNCTION__, vmm_info->root_table_hhdm);
        root_table_hh = (uint64_t *) vmm_info->root_table_hhdm;
    }

    vmm_info->vmmDataStart = align_value_to_page(higherHalfDirectMapBase + memory_size_in_bytes + VM_KERNEL_MEMORY_PADDING);

    vmm_info->status.vmm_container_root = (VmmContainer *) vmm_info->vmmDataStart;

    vmm_info->status.end_of_vmm_data = (uint64_t) vmm_container_root + VMM_RESERVED_SPACE_SIZE;

    //maybe start of vmm space can be removed.

    if (vmm_level == VMM_LEVEL_SUPERVISOR) {
        loglinef(Verbose, "(%s): Supervisor level initialization", __FUNCTION__);
        vmm_info->vmmSpaceStart = vmm_info->vmmDataStart + VMM_RESERVED_SPACE_SIZE + VM_KERNEL_MEMORY_PADDING;
        vmm_info->start_of_vmm_space = (size_t) vmm_info->status.vmm_container_root + VMM_RESERVED_SPACE_SIZE + VM_KERNEL_MEMORY_PADDING;
    } else if (vmm_level == VMM_LEVEL_USER) {
        loglinef(Verbose, "(%s): User level initialization", __FUNCTION__);
        vmm_info->vmmSpaceStart = 0x0l + VM_KERNEL_MEMORY_PADDING;
        vmm_info->start_of_vmm_space = 0x0l + VM_KERNEL_MEMORY_PADDING;
    } else {
        loglinef(Fatal, "(%s): Error: unsupported vmm privilege level", __FUNCTION__);
    }

    vmm_info->status.next_available_address = vmm_info->start_of_vmm_space;
    vmm_info->status.vmm_items_per_page = (PAGE_SIZE_IN_BYTES / sizeof(VmmItem)) - 1;
    vmm_info->status.vmm_cur_index = 0;

    loglinef(Verbose, "(%s): vmm_container_root starts at: 0x%x - %d", __FUNCTION__, vmm_info->status.vmm_container_root, is_address_aligned(vmm_info->vmmDataStart, PAGE_SIZE_IN_BYTES));
    loglinef(Verbose, "(%s): vmmDataStart  starts at: 0x%x - %x (end_of_vmm_data)", __FUNCTION__, vmm_info->vmmDataStart, vmm_info->status.end_of_vmm_data);
    loglinef(Verbose, "(%s): higherHalfDirectMapBase: %x, is_aligned: %d", __FUNCTION__, (uint64_t) higherHalfDirectMapBase, is_address_aligned(higherHalfDirectMapBase, PAGE_SIZE_IN_BYTES));
    loglinef(Verbose, "(%s): vmmSpaceStart: %x - start_of_vmm_space: (%x)", __FUNCTION__,  (uint64_t) vmm_info->vmmSpaceStart, vmm_info->start_of_vmm_space);
    loglinef(Verbose, "(%s): sizeof VmmContainer: 0x%x", __FUNCTION__, sizeof(VmmContainer));

    //I need to compute the size of the VMM address space
    void* vmm_root_phys = pmm_alloc_frame();
    if (vmm_root_phys == NULL) {
        loglinef(Verbose, "(vmm_init):  vmm_root_phys should not be null");
        return;
    }

    // Mapping the phyiscal address for the vmm structures
    map_phys_to_virt_addr_hh(vmm_root_phys, vmm_info->status.vmm_container_root, VMM_FLAGS_PRESENT | VMM_FLAGS_WRITE_ENABLE, root_table_hh);

    vmm_info->status.vmm_container_root->next = NULL;
    vmm_info->status.vmm_cur_container = vmm_info->status.vmm_container_root;
}

void *vmm_alloc(size_t size, size_t flags, VmmInfo *vmm_info) {

    if ( vmm_info == NULL ) {
        vmm_info = &vmm_kernel;
    }

    //TODO When the space inside this page is finished we need to allocate a new page
    //     at vmm_cur_container + sizeof(VmmItem)
    if (size == 0) {
        return NULL;
    }

    if (vmm_info->status.vmm_cur_index >= vmm_info->status.vmm_items_per_page) {
        // Step 1: We need to create another VmmContainer
        void *new_container_phys_address = pmm_alloc_frame();
        VmmContainer *new_container = NULL;
        if ( new_container_phys_address != NULL) {
            // 1.a We need to get the virtual address for the new structure
            new_container = (VmmContainer*)align_value_to_page((uint64_t)vmm_info->status.vmm_cur_container + sizeof(VmmContainer) + PAGE_SIZE_IN_BYTES);
            loglinef(Verbose, "(%s): new address 0x%x is aligned: %d", __FUNCTION__, new_container, is_address_aligned((uintptr_t)new_container, PAGE_SIZE_IN_BYTES));
            map_phys_to_virt_addr_hh(new_container_phys_address, new_container, VMM_FLAGS_PRESENT | VMM_FLAGS_WRITE_ENABLE, (uint64_t *) vmm_info->root_table_hhdm);
            // Step 2: Reset vmm_cur_index
            vmm_info->status.vmm_cur_index = 0;
            // Step 2.a: Set next as null for new_container;
            new_container->next = NULL;
            // Step 3: Add the new container as next item in the current one
            vmm_info->status.vmm_cur_container->next = new_container;
            // Step 4: make the new container as the current
            vmm_info->status.vmm_cur_container = new_container;
        } else {
            loglinef(Fatal, "(%s): pmm_alloc_frame for new container has returned null, this should not happen!", __FUNCTION__);
            return NULL;
        }
    }

    // Now i need to align the requested length to a page
    size_t new_size = align_value_to_page(size);
    loglinef(Verbose, "(%s): size: %d - aligned: %d", __FUNCTION__, size, new_size);

    uintptr_t address_to_return = vmm_info->status.next_available_address;
    vmm_info->status.vmm_cur_container->vmm_root[vmm_info->status.vmm_cur_index].base = address_to_return;
    vmm_info->status.vmm_cur_container->vmm_root[vmm_info->status.vmm_cur_index].flags = flags;
    vmm_info->status.vmm_cur_container->vmm_root[vmm_info->status.vmm_cur_index].size = new_size;
    vmm_info->status.next_available_address += new_size;

    if ( !is_address_higher_half(address_to_return) ) {
        flags = flags | VMM_FLAGS_USER_LEVEL;
    }

    loglinef(Verbose, "(%s): Flags PRESENT(%d) - WRITE(%d) - USER(%d)", __FUNCTION__, flags & VMM_FLAGS_PRESENT, flags & VMM_FLAGS_WRITE_ENABLE, flags & VMM_FLAGS_USER_LEVEL);

    if  (!is_address_only(flags) ) {
        loglinef(Verbose, "(%s): This means that we want the address to be mapped directly with physical memory.", __FUNCTION__);

        size_t required_pages = get_number_of_pages_from_size(size);
        size_t arch_flags = vm_parse_flags(flags);
        loglinef(Verbose, "(%s): Testing vm_parse_flags: 0x%x required pages: %d - address to ret: 0x%x - arch_flags: 0x%x", __FUNCTION__, arch_flags, required_pages, address_to_return, arch_flags);
        loglinef(Verbose, "(%s): address: 0x%x", __FUNCTION__, vmm_info->root_table_hhdm);

        for  ( size_t i = 0; i < required_pages; i++ )  {
            void *frame = pmm_alloc_frame();
            loglinef(Verbose, "(%s): address to map: 0x%x - phys frame: 0x%x", __FUNCTION__, frame, address_to_return);
            map_phys_to_virt_addr_hh((void*) frame, (void *)address_to_return + (i * PAGE_SIZE_IN_BYTES), arch_flags | VMM_FLAGS_PRESENT | VMM_FLAGS_WRITE_ENABLE, (uint64_t *) vmm_info->root_table_hhdm);
        }
    }

    //loglinef(Verbose, "(vmm_alloc) newly allocated item base: %x, next available address: %x", vmm_cur_container->vmm_root[vmm_cur_index].base, next_available_address);
    loglinef(Verbose, "(vmm_alloc) newly allocated item base: %x, next available address: %x", vmm_info->status.vmm_cur_container->vmm_root[vmm_info->status.vmm_cur_index].base, vmm_info->status.next_available_address);
    vmm_info->status.vmm_cur_index++;

    return (void *) address_to_return;
}

bool is_address_only(size_t  flags) {
    if(flags & VMM_FLAGS_ADDRESS_ONLY) {
        return true;
    }
    return false;
}

void vmm_free(void *address) {
    loglinef(Verbose, "(vmm_free) To Be implemented address provided is: 0x%x", address);
    VmmContainer *selected_container = vmm_container_root;

    if (vmm_items_per_page <= 0) {
        logline(Verbose, "(vmm_free) Error: vmm_items_per_page can't be equal or less than 0)");
        return;
    }

    while(selected_container != NULL) {
        selected_container = selected_container->next;
    }

    // Need to compute:
    // Page directories/table entries for the address
    // Search the base address inside the vmm objects arrays
    // Remove that object
    // Mark the entry as not present at least if it is mapped
    return;
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

//TODO implement this function or remove it
uint8_t check_virt_address_status(uint64_t virtual_address) {
    (void)virtual_address;
    return VIRT_ADDRESS_NOT_PRESENT;
}



