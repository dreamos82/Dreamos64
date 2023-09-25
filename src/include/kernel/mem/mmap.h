#ifndef _MMAP_H_
#define _MMAP_H_

#include <multiboot.h>
#include <stdint.h>
#include <stddef.h>

#define _MMAP_DEFECTIVE 5
#define _MMAP_NVS 4
#define _MMAP_RECLAIMABLE 3
#define _MMAP_RESERVED 2
#define _MMAP_AVAILABLE 1
#define _MMAP_INVALID 0

extern const char *mmap_types[];

void _mmap_parse(struct multiboot_tag_mmap*);
void _mmap_setup();
uintptr_t _mmap_determine_bitmap_region(uint64_t lower_limit, size_t size);
#endif
