#include <hh_direct_map.h>
#include <kernel.h>
#include <logging.h>
#include <mmap.h>
#include <vm.h>
#include <vmm.h>
#include <vmm_mapping.h>

extern uint64_t p4_table[];
extern uint64_t p3_table_hh[];
extern uint64_t p2_table[];
extern uint64_t pt_tables[];


/**
 * This function return the hhdm pointer of the physycal address provided
 *
 *
 * @param phys_address the physical address we want to retrieve
 * @return hhdm virtual address of the physical address or NULL in case of error
 */
void *hhdm_get_variable ( uintptr_t phys_address ) {
    if ( phys_address < memory_size_in_bytes) {
        //pretty_logf(Verbose, "Phys address: 0x%x Returning address: 0x%x", phys_address, (void *) (phys_address + higherHalfDirectMapBase));
        return (void*)(phys_address + higherHalfDirectMapBase);
    }
    pretty_logf(Verbose, "Not in physical memory. Faulting address: 0x%x", (uint64_t)phys_address);
    return NULL;
}

/**
 * This is an helper function return the physical address given a hhdm one
 *
 *
 * @return phys_address the physical address we want to retrieve
 * @param hhdm_address of the physical address or NULL in case of error
 */
void *hhdm_get_phys_address(uintptr_t hhdm_address) {
    return (void *)(hhdm_address - higherHalfDirectMapBase);
}


void hhdm_map_physical_memory() {
    // This function should be called only once, and the hhdm shouldn't change during the kernel uptime
    // This function needs to map the entire phyisical memory inside the virtual memory environment.
    // The starting address is _HIGHER_HALF_KERNEL_MEM_START
    uint64_t end_of_mapped_physical_memory = end_of_mapped_memory - _HIGHER_HALF_KERNEL_MEM_START;
    if (is_phyisical_address_mapped(end_of_mapped_physical_memory, end_of_mapped_physical_memory)) {
        end_of_mapped_memory = end_of_mapped_memory + PAGE_SIZE_IN_BYTES;
        end_of_mapped_physical_memory = end_of_mapped_physical_memory + PAGE_SIZE_IN_BYTES;
    }

    uint64_t address_to_map = 0;
    uint64_t virtual_address = higherHalfDirectMapBase;

    pretty_logf(Verbose, "Virtual address: 0x%x -- hhdmBase: 0x%x", virtual_address, higherHalfDirectMapBase);
    pretty_logf(Verbose, "Vaddress: 0x%x - HigherHalf Initial entries: pml4: %d, pdpr: %d, pd: %d", virtual_address, PML4_ENTRY((uint64_t) higherHalfDirectMapBase), PDPR_ENTRY((uint64_t) higherHalfDirectMapBase), PD_ENTRY((uint64_t) higherHalfDirectMapBase));

    size_t current_pml4_entry = PML4_ENTRY((uint64_t) higherHalfDirectMapBase);

    uint64_t upper_address_to_map = (mmap_entries[_mmap_last_available_item].addr + mmap_entries[_mmap_last_available_item].len);
    pretty_logf(Verbose, "Last available item: %d -  Last address to map: 0x%x", _mmap_last_available_item, upper_address_to_map);

    while ( address_to_map < upper_address_to_map) {
        //p4_table[current_entry] = address_to_map | HUGEPAGE_BIT| WRITE_BIT | PRESENT_BIT;
        map_phys_to_virt_addr((void*)address_to_map, (void*)virtual_address, VMM_FLAGS_PRESENT | VMM_FLAGS_WRITE_ENABLE);
        address_to_map += PAGE_SIZE_IN_BYTES;
        virtual_address += PAGE_SIZE_IN_BYTES;
    }

    // This is the kernel mapped in -2G
    pretty_logf(Verbose, "Physical memory mapped end: 0x%x - Virtual memory end: 0x%x - counter: %d", end_of_mapped_physical_memory, end_of_mapped_memory, current_pml4_entry);
    pretty_logf(Verbose, "hhdm end: 0x%x", virtual_address);
}
