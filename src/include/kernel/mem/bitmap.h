#ifndef _BITMAP_H
#define _BITMAP_H

#include <stdint.h>

#define PAGE_SIZE_IN_BYTES 0x200000
#if PAGE_SIZE == 2
    #define PAGE_SIZE_IN_BYTES 0x200000
#elif PAGE_SIZE == 4
    #define PAGE_SIZE_IN_BYTES 0x1000
#endif
#define FREE_TO_USE 0
#define USED 1
#define RESERVED 2

void _initialize_bitmap();

#endif
