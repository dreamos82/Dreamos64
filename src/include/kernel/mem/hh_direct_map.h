#ifndef __HH_DIRECT_MAP__
#define __HH_DIRECT_MAP__

#include <stddef.h>

void *hhdm_get_variable ( size_t phys_address );
void hhdm_map_physical_memory();

#endif
