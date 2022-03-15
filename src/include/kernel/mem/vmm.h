#ifndef __VMM_H
#define __VMM_H

#include <stddef.h>

#define PRESENT 1
#define WRITE_ENABLE 2
#define USER_LEVEL 4

void *map_vaddress(void *, unsigned int);
int unmap_vaddress(void *);

void *map_phys_to_virt_addr(void *, void *, unsigned int);
void map_vaddress_range(void *, unsigned int, size_t);
#endif
