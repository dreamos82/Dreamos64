#ifndef __VMM_H
#define __VMM_H

void _initialize_vmm();

void* request_virtual_page(size_t);
void free_virtual_page();

void map_vaddress(void *, int);

#endif
