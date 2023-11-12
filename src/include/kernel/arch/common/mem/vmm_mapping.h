#ifndef _VMM_MAPPING_H_
#define _VMM_MAPPING_H_

#include <stdint.h>

void *map_phys_to_virt_addr_hh(void* physical_address, void* address, size_t flags, uint64_t *pml4_root);
void *map_phys_to_virt_addr(void* physical_address, void* address, size_t flags);

void identity_map_phys_address(void *pyhysical_address, size_t flags);
void map_vaddress_range(void *virtual_address, size_t flags, size_t required_pages, uint64_t *pml4_root);

void *map_vaddress(void *address, size_t flags, uint64_t *pml4_root);
int unmap_vaddress(void *address);

#endif
