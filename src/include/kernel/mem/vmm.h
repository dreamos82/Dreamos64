#ifndef __VMM_H
#define __VMM_H

#include <stddef.h>
#include <bitmap.h>

//#define NONE 0
//#define PRESENT 0b1
//#define WRITE_ENABLE 2
//#define USER_LEVEL 4

#define PHYS_ADDRESS_NOT_MAPPED  0 // Address is not mapped 
#define PHYS_ADDRESS_MAPPED 0b1
#define PHYS_ADDRESS_MISMATCH 0b10 // This is returned when given a phys and virt address, the virt address does not contain the phys one

#define VIRT_ADDRESS_PRESENT 1
#define VIRT_ADDRESS_NOT_PRESENT 0

#define VM_KERNEL_MEMORY_PADDING PAGE_SIZE_IN_BYTES

#define VMM_RESERVED_SPACE_SIZE 0x14480000000

#define VPTR(x) (void*)((uint64_t)(x))

typedef enum {
    VMM_FLAGS_NONE = 0,
    VMM_FLAGS_PRESENT = (1 << 0),
    VMM_FLAGS_WRITE_ENABLE = (1 << 1),
    VMM_FLAGS_USER_LEVEL = (1 << 2),
    VMM_FLAGS_ADDRESS_ONLY = (1 << 7)
} paging_flags_t;

typedef struct VmmItem{
    uintptr_t base;
    size_t size;
    size_t flags;
} VmmItem;

/**
 * This struct contains the base addresses used by the Virtual Memory Manager
 */
typedef struct VmmInfo {
    uintptr_t higherHalfDirectMapBase; /**< The startr of the physical memory direct mapping */
    uintptr_t vmmDataStart; /**< The start of the VMM reserved area for it's own data structures */
    uintptr_t vmmSpaceStart; /**< The start of the VMM space, where all allocation will be placed */
} VmmInfo;

typedef struct VmmContainer {
    VmmItem vmm_root[(PAGE_SIZE_IN_BYTES/sizeof(VmmItem) - 1)];
    struct VmmContainer *next;
} __attribute__((__packed__)) VmmContainer;

extern uint64_t end_of_vmm_space;
extern VmmInfo vmm_info;

void vmm_init();

void *vmm_alloc(size_t size, size_t flags);
void vmm_free(void *address);

void *map_vaddress(void *address, paging_flags_t flags);
int unmap_vaddress(void *address);

void *map_phys_to_virt_addr(void* physical_address, void* address, paging_flags_t flags);
void identity_map_phys_address(void *pyhysical_address, paging_flags_t flags);
void map_vaddress_range(void *virtual_address, paging_flags_t flags, size_t required_pages);
uint8_t is_phyisical_address_mapped(uintptr_t physical_address, uintptr_t virtual_address);
uint8_t check_virt_address_status(uint64_t virtual_address);
void direct_map_physical_memory();

bool is_address_only(paging_flags_t flags);

#endif
