#ifndef __MAIN_H_
#define __MAIN_H_

#include <stdint.h>

#define _HIGHER_HALF_KERNEL_MEM_START   0xffffffff80000000
void _read_configuration_from_multiboot(unsigned long);
void kernel_start(unsigned long addr, unsigned long);

#endif
