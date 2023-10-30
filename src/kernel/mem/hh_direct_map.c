#include <hh_direct_map.h>
#include <kernel.h>
#include <logging.h>
#include <vmm.h>
#include <vmm_mapping.h>

/**
 * This function return the hhdm pointer of the physycal address provided
 *
 *
 * @param phys_address the physical address we want to retrieve
 * @return hhdm virtual address of the physical address or NULL in case of error
 */
void *hhdm_get_variable ( size_t phys_address ) {
    if ( phys_address < memory_size_in_bytes) {
        loglinef(Verbose, "(%s): Returning address: 0x%x", __FUNCTION__, phys_address + higherHalfDirectMapBase);
        return (void*)(phys_address + higherHalfDirectMapBase);
    }
    loglinef(Verbose, "(%s): Not in physical memory", __FUNCTION__);
    return NULL;
}


void hhdm_map_physical_memory() {
    // This function needs to map the entire phyisical memory inside the virtual memory enironment.
    // The starting address is _HIGHER_HALF_KERNEL_MEM_START
    loglinef(Verbose, "(direct_map_physical_memory) End of memory_mapping phys: 0x%x, memory_size: 0x%x", end_of_mapped_memory - _HIGHER_HALF_KERNEL_MEM_START, memory_size_in_bytes);
    uint64_t end_of_mapped_physical_memory = end_of_mapped_memory - _HIGHER_HALF_KERNEL_MEM_START;
    if (is_phyisical_address_mapped(end_of_mapped_physical_memory, end_of_mapped_physical_memory)) {
        end_of_mapped_memory = end_of_mapped_memory + PAGE_SIZE_IN_BYTES;
        end_of_mapped_physical_memory = end_of_mapped_physical_memory + PAGE_SIZE_IN_BYTES;
    }

    uint64_t address_to_map = 0;
    uint64_t virtual_address = higherHalfDirectMapBase;

    while ( address_to_map < memory_size_in_bytes) {
        map_phys_to_virt_addr((void*)address_to_map, (void*)virtual_address, VMM_FLAGS_PRESENT | VMM_FLAGS_WRITE_ENABLE, NULL);
        address_to_map += PAGE_SIZE_IN_BYTES;
        virtual_address += PAGE_SIZE_IN_BYTES;
        //loglinef(Verbose, "(direct_map_physical_memory) Mapping physical address: 0x%x - To virtual: 0x%x", address_to_map, virtual_address);
    }
    loglinef(Verbose, "(direct_map_physical_memory) Physical memory mapped end: 0x%x - Virtual memory direct end: 0x%x", end_of_mapped_physical_memory, end_of_mapped_memory);
}
