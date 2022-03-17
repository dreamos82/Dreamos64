#ifndef __VMM_H
#define __VMM_H

#include <stddef.h>

#define PRESENT 0b1
#define WRITE_ENABLE 2
#define USER_LEVEL 4

void *map_vaddress(void *, unsigned int);
int unmap_vaddress(void *);

void *map_phys_to_virt_addr(void *, void *, unsigned int);
void map_vaddress_range(void *, unsigned int, size_t);
bool is_phyisical_address_mapped(uint64_t, uint64_t);
#endif
