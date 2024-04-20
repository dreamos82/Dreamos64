#ifndef __MAIN_H_
#define __MAIN_H_

#include <stdint.h>
#include <multiboot.h>

extern struct multiboot_tag *tag_start;

void _init_basic_system(unsigned long addr);
void kernel_start(unsigned long addr, unsigned long magic);

#endif
