#ifndef __HH_DIRECT_MAP__
#define __HH_DIRECT_MAP__

#include <stddef.h>
#include <stdint.h>

// Same as kernel KERNEL_MEMORY_PADDING, that is in kheap.h, they will be merged once the memory initialization is fixed
#define HH_MEMORY_PADDING 0x1000

// This function is temporary
void early_map_physical_memory(uint64_t end_of_reserved_area);

void *hhdm_get_variable ( uintptr_t phys_address );
void hhdm_map_physical_memory();

#endif
