#ifndef _BITMAP_H
#define _BITMAP_H

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

#if SMALL_PAGES == 0
    #define PAGE_SIZE_IN_BYTES 0x200000
#elif SMALL_PAGES == 1
    #define PAGE_SIZE_IN_BYTES 0x1000
#endif

#define BITMAP_ENTRY_FULL 0xfffffffffffffff
#define BITMAP_ROW_BITS 64

#define FREE_TO_USE 0
#define USED 1

#define ADDRESS_TO_BITMAP_ENTRY(address)(address / PAGE_SIZE_IN_BYTES)

void _initialize_bitmap(unsigned long addr);

int64_t _bitmap_request_frame();
void _bitmap_set_bit(uint64_t);
void _bitmap_free_bit(uint64_t);
bool _bitmap_test_bit(uint64_t);
uint32_t _compute_kernel_entries();

#endif
