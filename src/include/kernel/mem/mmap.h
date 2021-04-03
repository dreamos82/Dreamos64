#ifndef _MMAP_H
#define _MMAP_H

#include <multiboot.h>
#include <stdint.h>

typedef struct {
    uint32_t number_of_entries;
    multiboot_memory_map_t *entries;
} mmap_wrapper;

char *mmap_types[5] = {
    "Invalid",
    "Available",
    "Reserved",
    "Reclaimable",
    "NVS",
    "Defective"
};
void _parse_mmap(struct multiboot_tag_mmap*);

#endif
