#ifndef _UTIL_H
#define _UTIL_H

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

#define READMEM32(addr) \
(*(volatile uint32_t*)((uintptr_t)addr))

#define READMEM64(addr) \
(*(volatile uint64_t*)((uintptr_t)addr))

#define WRITEMEM32(addr, u32) \
(*(volatile uint32_t*)((uintptr_t)addr)) = u32

#define WRITEMEM64(addr, u64) \
(*(volatile uint64_t*)((uintptr_t)addr)) = u64

size_t get_number_of_pages_from_size(size_t size);
size_t align_value_to_page(size_t value);
size_t align_up(size_t value, size_t alignment);
bool is_address_aligned(size_t value, size_t alignment);

#endif
