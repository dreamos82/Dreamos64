#include <hh_direct_map.h>
#include <kernel.h>
#include <logging.h>
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
        pretty_logf(Verbose, "Phys address: 0x%x Returning address: 0x%x", phys_address, (void *) (phys_address + higherHalfDirectMapBase));
        return (void*)(phys_address + higherHalfDirectMapBase);
    }
    pretty_logf(Verbose, "(%s): Not in physical memory. Faulting address: 0x%x", (uint64_t)phys_address);
    return NULL;
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

    pretty_logf(Verbose, "HigherHalf Initial entries: pml4: %d, pdpr: %d, pd: %d", PML4_ENTRY((uint64_t) higherHalfDirectMapBase), PDPR_ENTRY((uint64_t) higherHalfDirectMapBase), PD_ENTRY((uint64_t) higherHalfDirectMapBase));

    size_t current_pml4_entry = PML4_ENTRY((uint64_t) higherHalfDirectMapBase);

    if (!(p4_table[current_pml4_entry] & 0b1) ) {
        pretty_log(Fatal, "This shouldn't happen");
    }

    while ( address_to_map < memory_size_in_bytes) {
        //loglinef(Verbose, "(%s) Mapping physical address: 0x%x - virtual address: 0x%x", __FUNCTION__ ,address_to_map, virtual_address);
        //p4_table[current_entry] = address_to_map | HUGEPAGE_BIT| WRITE_BIT | PRESENT_BIT;
        map_phys_to_virt_addr((void*)address_to_map, (void*)virtual_address, VMM_FLAGS_PRESENT | VMM_FLAGS_WRITE_ENABLE);
        address_to_map += PAGE_SIZE_IN_BYTES;
        virtual_address += PAGE_SIZE_IN_BYTES;
    }

    pretty_logf(Verbose, "Physical memory mapped end: 0x%x - Virtual memory direct end: 0x%x - counter: %d", end_of_mapped_physical_memory, end_of_mapped_memory, current_pml4_entry);

}
