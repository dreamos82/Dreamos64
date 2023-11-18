#ifndef __HH_DIRECT_MAP__
#define __HH_DIRECT_MAP__

#include <stddef.h>
#include <stdint.h>

void *hhdm_get_variable ( uintptr_t phys_address );
void hhdm_map_physical_memory();

#endif
