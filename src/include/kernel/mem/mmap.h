#ifndef _MMAP_H_
#define _MMAP_H_

#include <multiboot.h>
#include <stdint.h>

typedef struct {
    uint32_t number_of_entries;
    multiboot_memory_map_t *entries;
} mmap_wrapper;

extern const char *mmap_types[];

void _mmap_parse(struct multiboot_tag_mmap*);
void _mmap_setup();
#endif
