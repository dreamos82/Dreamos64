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
    uintptr_t vmmDataStart; /**< The start of the VMM reserved area for it's own data structures */
    uintptr_t vmmSpaceStart; /**< The start of the VMM space, where all allocation will be placed */

    size_t start_of_vmm_space; /**< The starting addres ofthe vmm space */

    uintptr_t pml4_root;

    struct VmmStatus {
        size_t vmm_items_per_page; /**< Number of page items contained in one page */
        size_t vmm_cur_index; /**< Current position inside the array */

        size_t next_available_address; /**< The next available address */

        uint64_t end_of_vmm_data; /**< We should never reach here, where the vmm_data finish */

        VmmContainer *vmm_container_root; /**< Root node of the vmmContainer */
        VmmContainer *vmm_cur_container; /**< Current pointer */
    } status;
} VmmInfo;

//uint64_t memory_size_in_bytes;
extern uint64_t end_of_mapped_memory;
extern uint64_t end_of_vmm_space;
extern VmmInfo vmm_info;
extern uintptr_t higherHalfDirectMapBase; /**< The start of the physical memory direct mapping */

void vmm_init(vmm_level_t vmm_level, VmmInfo *vmm_info);

void *vmm_alloc(size_t size, size_t flags, VmmInfo *vmm_info);
void vmm_free(void *address);

uint8_t is_phyisical_address_mapped(uintptr_t physical_address, uintptr_t virtual_address);
uint8_t check_virt_address_status(uint64_t virtual_address);
void vmm_direct_map_physical_memory();

bool is_address_only(size_t  flags);

void *vmm_get_variable_from_direct_map ( size_t phys_address );

#endif
