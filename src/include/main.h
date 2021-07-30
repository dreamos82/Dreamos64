#ifndef __MAIN_H_
#define __MAIN_H_

#include <stdint.h>

#define _HIGHER_HALF_KERNEL_MEM_START   0xffffffff80000000
void _init_basic_system(unsigned long);
void kernel_start(unsigned long addr, unsigned long);

#endif
