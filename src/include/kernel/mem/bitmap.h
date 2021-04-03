#ifndef _BITMAP_H
#define _BITMAP_H

#include <stdint.h>

#define PAGE_SIZE 4

#define FREE_TO_USE 0
#define USED 1
#define RESERVED 2

void _initialize_bitmap();

#endif
