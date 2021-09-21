#ifndef __VMM_H
#define __VMM_H

#include <stddef.h>

#define PRESENT 1
#define WRITE_ENABLE 2
#define USER_LEVEL 4

void _initialize_vmm();

void* request_virtual_page(size_t);
void free_virtual_page();

void *map_vaddress(void *, unsigned int);
void unmap_vaddress(void *);

#endif
