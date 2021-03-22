#ifndef __MAIN_H_
#define __MAIN_H_

#include <stdint.h>
void _read_configuration_from_multiboot(unsigned long);
void kernel_start(unsigned long addr, unsigned long);

#endif
