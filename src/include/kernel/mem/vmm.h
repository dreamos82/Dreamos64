#ifndef __VMM_H
#define __VMM_H

#include <stddef.h>

#define NONE 0
#define PRESENT 0b1
#define WRITE_ENABLE 2
#define USER_LEVEL 4

#define PHYS_ADDRESS_NOT_MAPPED  0 // Address is not mapped 
#define PHYS_ADDRESS_MAPPED 0b1
#define PHYS_ADDRESS_MISMATCH 0b10 // This is returned when given a phys and virt address, the virt address does not contain the phys one

#define VIRT_ADDRESS_PRESENT 1
#define VIRT_ADDRESS_NOT_PRESENT 0

#define VM_KERNEL_MEMORY_PADDING 0x1000

#define VMM_RESERVED_SPACE_SIZE 0x14480000000

#define VPTR(x) (void*)((uint64_t)(x))

typedef struct VmmItem{
    uintptr_t base;
    size_t size;
    size_t flags;
    struct VmmItem* next;
} VmmItem;

extern uint64_t end_of_vmm_space;

void vmm_init();

void *vmm_alloc();

void *map_vaddress(void *, unsigned int);
int unmap_vaddress(void *);

void *map_phys_to_virt_addr(void* physical_address, void* address, unsigned int flags);
void identity_map_phys_address(void *pyhysical_address, unsigned int flags);
void map_vaddress_range(void *virtual_address, unsigned int flags, size_t required_pages);
uint8_t is_phyisical_address_mapped(uint64_t physical_address, uint64_t virtual_address);
uint8_t check_virt_address_status(uint64_t virtual_address);
#endif
