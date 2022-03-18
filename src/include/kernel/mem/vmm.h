#ifndef __VMM_H
#define __VMM_H

#include <stddef.h>

#define PRESENT 0b1
#define WRITE_ENABLE 2
#define USER_LEVEL 4

#define PHYS_ADDRESS_NOT_MAPPED  0 // Address is not mapped 
#define PHYS_ADDRESS_MAPPED 0b1
#define PHYS_ADDRESS_MISMATCH 0b10 // This is returned when given a phys and virt address, the virt address does not contain the phys one

void *map_vaddress(void *, unsigned int);
int unmap_vaddress(void *);

void *map_phys_to_virt_addr(void *, void *, unsigned int);
void map_vaddress_range(void *, unsigned int, size_t);
uint8_t is_phyisical_address_mapped(uint64_t, uint64_t);
#endif
