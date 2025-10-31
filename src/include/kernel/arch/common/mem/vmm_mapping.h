#ifndef _VMM_MAPPING_H_
#define _VMM_MAPPING_H_

#include <stdint.h>

/**
 * This struct contains the result of a VM Page Tables walk
 */
typedef struct {
    uint64_t* pte; /**< The pointer to the entry on the leaf table containing the physical address requested */
    uint8_t level; /**< The level reached by the function */
    bool completed; /**< If false the requet did not completed. */
} vm_walk_results;

void *map_phys_to_virt_addr_hh(void* physical_address, void* address, size_t flags, uint64_t *pml4_root);
void *map_phys_to_virt_addr(void* physical_address, void* address, size_t flags);

void identity_map_phys_address(void *pyhysical_address, size_t flags);
void map_vaddress_range(void *virtual_address, size_t flags, size_t required_pages, uint64_t *pml4_root);

void *map_vaddress(void *address, size_t flags, uint64_t *pml4_root);
int unmap_vaddress(void *address);
int unmap_vaddress_hh(void *address, uint64_t *pml4_root);

uint8_t is_phyisical_address_mapped(uintptr_t physical_address, uintptr_t virtual_address);

uintptr_t vm_copy_from_different_space(uintptr_t virtual_address, uint64_t *root_table_hhdm);

vm_walk_results vm_walk_table(void* virtual_address, uint64_t *root_table_hhdm);

#endif
