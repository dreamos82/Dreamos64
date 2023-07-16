#ifndef __VMM_H
#define __VMM_H

#include <stddef.h>
#include <bitmap.h>

#define PHYS_ADDRESS_NOT_MAPPED  0 // Address is not mapped 
#define PHYS_ADDRESS_MAPPED 0b1
#define PHYS_ADDRESS_MISMATCH 0b10 // This is returned when given a phys and virt address, the virt address does not contain the phys one

//#define VMM_FLAGS_NONE = 0
//#define VMM_FLAGS_PRESENT = (1 << 0)
//#define VMM_FLAGS_WRITE_ENABLE = (1 << 1)
//#define VMM_FLAGS_USER_LEVEL = (1 << 2)
//#define VMM_FLAGS_ADDRESS_ONLY = (1 << 7)

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

typedef enum {
    VMM_LEVEL_USER,
    VMM_LEVEL_SUPERVISOR
} vmm_level_t;

typedef struct VmmItem{
    uintptr_t base;
    size_t size;
    size_t flags;
} VmmItem;

typedef struct VmmContainer {
    VmmItem vmm_root[(PAGE_SIZE_IN_BYTES/sizeof(VmmItem) - 1)];
    struct VmmContainer *next;
} __attribute__((__packed__)) VmmContainer;

/**
 * This struct contains the base addresses used by the Virtual Memory Manager
 */
typedef struct VmmInfo {
    uintptr_t higherHalfDirectMapBase; /**< The startr of the physical memory direct mapping */
    uintptr_t vmmDataStart; /**< The start of the VMM reserved area for it's own data structures */
    uintptr_t vmmSpaceStart; /**< The start of the VMM space, where all allocation will be placed */
    
    size_t start_of_vmm_space;
    
    // The variables below will containt the status information of the loaded vmm
    
    struct VmmStatus {
    
        VmmContainer *vmm_container_root;
        VmmContainer *vmm_cur_container;
        
        size_t vmm_items_per_page;
        size_t vmm_cur_index;
                
        size_t next_available_address;
    } status;
    
} VmmInfo;

extern uint64_t end_of_vmm_space;
extern VmmInfo vmm_info;

void vmm_init(vmm_level_t vmm_level, VmmInfo *task_vmm_info);

void *vmm_alloc(size_t size, size_t flags);
void vmm_free(void *address);

void *map_vaddress(void *address, size_t flags);
int unmap_vaddress(void *address);

void *map_phys_to_virt_addr(void* physical_address, void* address, size_t flags);
void identity_map_phys_address(void *pyhysical_address, size_t flags);
void map_vaddress_range(void *virtual_address, size_t flags, size_t required_pages);
uint8_t is_phyisical_address_mapped(uintptr_t physical_address, uintptr_t virtual_address);
uint8_t check_virt_address_status(uint64_t virtual_address);
void vmm_direct_map_physical_memory();

bool is_address_only(size_t flags);

void *vmm_get_variable_from_direct_map ( size_t phys_address );

#endif
