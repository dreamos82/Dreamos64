#include <bitmap.h>
#include <kernel.h>
#include <logging.h>
#include <main.h>
#include <pmm.h>
#include <thread.h>
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

//size_t vmm_items_per_page;
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

    if ( vmm_info == NULL ) {
        pretty_log(Verbose, "Kernel vmm initialization");
        vmm_info = &vmm_kernel;
        root_table_hh = kernel_settings.paging.hhdm_page_root_address;
    } else {
        pretty_logf(Verbose, "Task vmm initialization: root_table_hhdm: 0x%x- Phys: %x", vmm_info->root_table_hhdm, vmm_info->root_table_phys);
        root_table_hh = (uint64_t *) vmm_info->root_table_hhdm;
    }

    vmm_info->vmmDataStart = align_value_to_page(higherHalfDirectMapBase + memory_size_in_bytes + VM_KERNEL_MEMORY_PADDING);

    vmm_info->status.vmm_container_root = (VmmContainer *) vmm_info->vmmDataStart;

    vmm_info->status.end_of_vmm_data = (uint64_t) vmm_container_root + VMM_RESERVED_SPACE_SIZE;

    //maybe start of vmm space can be removed.

    if (vmm_level == VMM_LEVEL_SUPERVISOR) {
        pretty_log(Verbose, "Supervisor level initialization");
        vmm_info->vmmSpaceStart = vmm_info->vmmDataStart + VMM_RESERVED_SPACE_SIZE + VM_KERNEL_MEMORY_PADDING;
        vmm_info->start_of_vmm_space = (size_t) vmm_info->status.vmm_container_root + VMM_RESERVED_SPACE_SIZE + VM_KERNEL_MEMORY_PADDING;
    } else if (vmm_level == VMM_LEVEL_USER) {
        pretty_log(Verbose, "User level initialization");
        vmm_info->vmmSpaceStart = 0x0l + VM_KERNEL_MEMORY_PADDING;
        vmm_info->start_of_vmm_space = 0x0l + VM_KERNEL_MEMORY_PADDING;
    } else {
        pretty_log(Fatal, "Error: unsupported vmm privilege level");
    }

    vmm_info->status.next_available_address = vmm_info->start_of_vmm_space;
    vmm_info->status.vmm_items_per_page = (PAGE_SIZE_IN_BYTES / sizeof(VmmItem)) - 1;
    vmm_info->status.vmm_cur_index = 0;

    pretty_logf(Verbose, "\tvmm_container_root starts at: 0x%x - %x", vmm_info->status.vmm_container_root, is_address_aligned(vmm_info->vmmDataStart, PAGE_SIZE_IN_BYTES));
    pretty_logf(Verbose, "\tvmmDataStart  starts at: 0x%x - %x (end_of_vmm_data)", vmm_info->vmmDataStart, vmm_info->status.end_of_vmm_data);
    pretty_logf(Verbose, "\thigherHalfDirectMapBase: %x", (uint64_t) higherHalfDirectMapBase, is_address_aligned(higherHalfDirectMapBase, PAGE_SIZE_IN_BYTES));
    pretty_logf(Verbose, "\tvmmSpaceStart: %x - start_of_vmm_space: (%x)", (uint64_t) vmm_info->vmmSpaceStart, vmm_info->start_of_vmm_space);
    pretty_logf(Verbose, "\tsizeof VmmContainer: 0x%x - Number of items: %d", sizeof(VmmContainer), vmm_info->status.vmm_items_per_page);

    //I need to compute the size of the VMM address space
    void* vmm_root_phys = pmm_alloc_frame();
    if (vmm_root_phys == NULL) {
        pretty_log(Verbose, "vmm_root_phys should not be null");
        return;
    }

    // Mapping the phyiscal address for the vmm structures
    map_phys_to_virt_addr_hh(vmm_root_phys, vmm_info->status.vmm_container_root, VMM_FLAGS_PRESENT | VMM_FLAGS_WRITE_ENABLE, root_table_hh);
    vmm_info->status.vmm_container_root->next = NULL;
    vmm_info->status.vmm_container_root->vmm_items_available = vmm_info->status.vmm_items_per_page;
    vmm_info->status.vmm_number_of_containers = 1;
    vmm_info->status.vmm_cur_container = vmm_info->status.vmm_container_root;
}

void *vmm_alloc_at(uint64_t base_address, size_t size, size_t flags, VmmInfo *vmm_info) {

    if ( vmm_info == NULL ) {
        vmm_info = &vmm_kernel;
    }

    if (size == 0) {
        return NULL;
    }

    //TODO When the space inside this page is finished we need to allocate a new page
    //     at vmm_cur_container + sizeof(VmmItem)
    if (vmm_info->status.vmm_cur_index >= vmm_info->status.vmm_items_per_page) {
        // Step 1: We need to create another VmmContainer
        void *new_container_phys_address = pmm_alloc_frame();
        VmmContainer *new_container = NULL;
        if ( new_container_phys_address != NULL) {
            // 1.a We need to get the virtual address for the new structure
            new_container = (VmmContainer*)align_value_to_page((uint64_t)vmm_info->status.vmm_cur_container + sizeof(VmmContainer) + PAGE_SIZE_IN_BYTES);
            pretty_logf(Verbose, "new address 0x%x is aligned: %d", new_container, is_address_aligned((uintptr_t)new_container, PAGE_SIZE_IN_BYTES));
            map_phys_to_virt_addr_hh(new_container_phys_address, new_container, VMM_FLAGS_PRESENT | VMM_FLAGS_WRITE_ENABLE, (uint64_t *) vmm_info->root_table_hhdm);
            // Step 2: Reset vmm_cur_index
            vmm_info->status.vmm_cur_index = 0;
            // Step 2.a: Set next as null for new_container;
            new_container->next = NULL;
            new_container->vmm_items_available = vmm_info->status.vmm_items_per_page;
            // Step 3: Add the new container as next item in the current one
            vmm_info->status.vmm_cur_container->next = new_container;
            // Step 4: make the new container as the current
            vmm_info->status.vmm_cur_container = new_container;
            vmm_info->status.vmm_number_of_containers++;
        } else {
            pretty_log(Fatal, "pmm_alloc_frame for new container has returned null, this should not happen!");
            return NULL;
        }
    }

    // Now i need to align the requested length to a page
    size_t new_size = align_value_to_page(size);

    uintptr_t address_to_return = vmm_info->status.next_available_address;
    if (base_address != 0 && base_address > address_to_return) {
        // I have specified a base_address, so i want an allocation at that given address
        // This design is problematic, it will be reimplemented in the future
        // For now i rely on the fact that the address space on a 64bit architecture is very big. And i don't  worry about holes, or overlapping.
        if ( !is_address_aligned(base_address, PAGE_SIZE_IN_BYTES) ) {
            pretty_logf(Fatal, " Error: base_address 0x%x is not aligned with: 0x%x", base_address, PAGE_SIZE_IN_BYTES);
        }
        pretty_logf(Verbose, " Allocating address: 0x%x" , base_address);
        vmm_info->status.next_available_address = base_address;
        address_to_return = base_address;
    }

    vmm_info->status.vmm_cur_container->vmm_root[vmm_info->status.vmm_cur_index].base = address_to_return;
    vmm_info->status.vmm_cur_container->vmm_root[vmm_info->status.vmm_cur_index].flags = flags;
    vmm_info->status.vmm_cur_container->vmm_root[vmm_info->status.vmm_cur_index].size = new_size;
    vmm_info->status.vmm_cur_container->vmm_items_available--;
    vmm_info->status.next_available_address += new_size;

    if ( !is_address_higher_half(address_to_return) ) {
        flags = flags | VMM_FLAGS_USER_LEVEL;
    }

    pretty_logf(Verbose, "Flags PRESENT(%d) - WRITE(%d) - USER(%d)", flags & VMM_FLAGS_PRESENT, flags & VMM_FLAGS_WRITE_ENABLE, flags & VMM_FLAGS_USER_LEVEL);

    if  ( !is_address_only(flags) ) {

        size_t required_pages = get_number_of_pages_from_size(size);
        size_t arch_flags = vm_parse_flags(flags);

        pretty_logf(Verbose, "No physical memory needed: mapping address: 0x%x", vmm_info->root_table_hhdm);

        for  ( size_t i = 0; i < required_pages; i++ )  {
            void *frame = pmm_alloc_frame();
            pretty_logf(Verbose, "address to map: 0x%x - phys frame: 0x%x", frame, address_to_return);
            map_phys_to_virt_addr_hh((void*) frame, (void *)address_to_return + (i * PAGE_SIZE_IN_BYTES), arch_flags | VMM_FLAGS_PRESENT | VMM_FLAGS_WRITE_ENABLE, (uint64_t *) vmm_info->root_table_hhdm);
        }
    }

    //pretty_logf(Verbose, "(vmm_alloc) newly allocated item base: %x, next available address: %x", vmm_cur_container->vmm_root[vmm_cur_index].base, next_available_address);
    //pretty_logf(Verbose, "newly allocated item base: %x, next available address: %x", vmm_info->status.vmm_cur_container->vmm_root[vmm_info->status.vmm_cur_index].base, vmm_info->status.next_available_address);
    vmm_info->status.vmm_cur_index++;

    if ( is_address_stack(flags) ) {
        pretty_log(Verbose, "The address will be a stack");
        return (void *) address_to_return + THREAD_DEFAULT_STACK_SIZE;
    }

    return (void *) address_to_return;
}

void *vmm_alloc(size_t size, size_t flags, VmmInfo *vmm_info) {
    // 1. The address should be page aligned
    // 2. check if possible to reuse the vmm_alloc
    return vmm_alloc_at(0, size, flags, vmm_info);

}

bool is_address_only(size_t  flags) {
    if ( flags & VMM_FLAGS_ADDRESS_ONLY ) {
        return true;
    }
    return false;
}

bool is_address_stack(size_t flags) {
    if ( flags & VMM_FLAGS_STACK ) {
        return true;
    }
    return false;
}

void vmm_free(void *address, bool is_stack, VmmInfo *vmm_info) {
    //TODO: not finished yet
    pretty_logf(Verbose, "To Be implemented address provided is: 0x%x", address);
    if ( address == NULL ) {
        return;
    }
    if ( vmm_info == NULL ) {
        vmm_info = &vmm_kernel;
    }

    // TODO: check if the address is page aligned.

    VmmContainer *selected_container = vmm_info->status.vmm_container_root;
    VmmContainer *prev_container = selected_container;
    size_t vmm_items_per_page = vmm_info->status.vmm_items_per_page;

    if (vmm_items_per_page <= 0) {
        pretty_log(Verbose, "Error: vmm_items_per_page can't be equal or less than 0");
        return;
    } else {
        pretty_logf(Verbose, "Vmm_items_per_page: %d", vmm_info->status.vmm_items_per_page);
    }

    VmmContainer *container_containing_address = NULL;
    pretty_logf(Verbose, "selected_container null?: %d", selected_container == NULL);

    // Need to make a double check: if the address is a stack, I need to look for address - THREAD_DEFAULT_STACK_SIZE otherwise for address.

    while( selected_container != NULL ) {
        uintptr_t container_base = selected_container->vmm_root[0].base;
        if ( container_base < (uintptr_t) address ) {
            container_containing_address = selected_container;
            pretty_logf(Verbose, "\tcontainer_base: %x", container_base);
        } else {
            // TODO: Set item to 0 (not necessary, but useful
            break;
        }
        prev_container = selected_container;
        selected_container = selected_container->next;
    }

    pretty_logf(Verbose, "\tselected_container base: %x", container_containing_address->vmm_root[0].base);

    if ( is_stack ) {
        address = address - THREAD_DEFAULT_STACK_SIZE;
    }

    for ( size_t i = 0; i < vmm_items_per_page; i++ ) {
        if ( container_containing_address != NULL) {
            if ( container_containing_address->vmm_root[i].base == address) {
                pretty_logf(Verbose, "[%d] - Address found: base: %x - flags: %x - size: %d - available: %d", i, container_containing_address->vmm_root[0].base, container_containing_address->vmm_root[i].flags, container_containing_address->vmm_root[i].size, vmm_info->status.vmm_cur_container->vmm_items_available);
                vmm_info->status.vmm_cur_container->vmm_items_available++;
                pretty_logf(Verbose, "Updated value: %d", vmm_info->status.vmm_cur_container->vmm_items_available);
                break;
            }
        }
    }

    // Now let's check if I have freed all the items in the current container
    if ( vmm_info->status.vmm_cur_container->vmm_items_available == vmm_info->status.vmm_items_per_page ) {
        pretty_log(Verbose, "Container Will be deleted");
        // If we are here it means the container can be deleted.
        // CASE 1: There is only one container, it can't be eliminated. Let's just reset the index, and start from scratch
        if ( vmm_info->status.vmm_number_of_containers == 1) {
            vmm_info->status.vmm_cur_index = 0;
        } else {
        // CASE 2: we have more than one container, so we can safely delete the current one
        // and merge its previous container with the next container
        // Since i'm removing a container i need to decrease their total number
        // This number is needed to check if I have only one container left
        // Maybe is not really needed, i Just need to check if the current node is the root, and next is null.
            vmm_info->status.vmm_number_of_containers--;
            prev_container->next = selected_container->next;
        }
    }

    return;
}

//TODO implement this function or remove it
uint8_t check_virt_address_status(uint64_t virtual_address) {
    (void)virtual_address;
    return VIRT_ADDRESS_NOT_PRESENT;
}
