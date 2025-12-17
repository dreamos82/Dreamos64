#ifndef _BITMAP_H
#define _BITMAP_H

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

#if SMALL_PAGES == 0
    #define PAGE_SIZE_IN_BYTES 0x200000l
#elif SMALL_PAGES == 1
    #define PAGE_SIZE_IN_BYTES 0x1000l
#endif

#define BITMAP_ENTRY_FULL 0xfffffffffffffff
#define BITMAP_ROW_BITS 64

#define FREE_TO_USE 0
#define USED 1

#define ADDRESS_TO_BITMAP_ENTRY(address)(address / PAGE_SIZE_IN_BYTES)

typedef enum  {
    ADDRESS_TYPE_PHYSICAL,
    ADDRESS_TYPE_VIRTUAL
} address_type_t;

extern size_t memory_size_in_bytes;
extern uint64_t memory_map_phys_addr;

void _initialize_bitmap(uint64_t memory_size, uint64_t end_of_reserved_area);
void _bitmap_get_region(uint64_t* base_address, size_t* length_in_bytes, address_type_t type);

int64_t _bitmap_request_frame();
int64_t _bitmap_request_frames(size_t number_of_frames);
void _bitmap_set_bit(uint64_t location);
void _bitmap_free_bit(uint64_t location);
bool _bitmap_test_bit(uint64_t location);
void _bitmap_set_bit_from_address(uint64_t address);
uint32_t _compute_kernel_entries(uint64_t);

#endif
