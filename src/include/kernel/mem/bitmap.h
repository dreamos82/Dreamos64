#ifndef _BITMAP_H
#define _BITMAP_H

#include <stdint.h>
#include <stddef.h>

#if PAGE_SIZE == 2
    #define PAGE_SIZE_IN_BYTES 0x200000
#elif PAGE_SIZE == 4
    #define PAGE_SIZE_IN_BYTES 0x1000
#endif

#define BITMAP_ENTRY_FULL 0xfffffffffffffff
#define BITMAP_ROW_BITS 64

#define FREE_TO_USE 0
#define USED 1

void _initialize_bitmap();

uint64_t _bitmap_request_frame();
void _bitmap_set_bit(uint64_t);
void _bitmap_free_bit(uint64_t);
void _bitmap_test_bit(uint64_t);

#endif
