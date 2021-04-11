#ifndef _MMAP_H_
#define _MMAP_H_

#include <multiboot.h>
#include <stdint.h>

typedef struct {
    uint32_t number_of_entries;
    multiboot_memory_map_t *entries;
} mmap_wrapper;

extern const char *mmap_types[];

void _parse_mmap(struct multiboot_tag_mmap*);

#endif
