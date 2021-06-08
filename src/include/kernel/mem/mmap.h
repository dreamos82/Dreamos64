#ifndef _MMAP_H_
#define _MMAP_H_

#include <multiboot.h>
#include <stdint.h>

extern const char *mmap_types[];

void _mmap_parse(struct multiboot_tag_mmap*);
void _mmap_setup();
#endif
