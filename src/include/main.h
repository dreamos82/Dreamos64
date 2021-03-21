#ifndef __MAIN_H_
#define __MAIN_H_

void _read_configuration_from_multiboot(unsigned long);
void kernel_start(unsigned long addr, unsigned long);

extern _cpuid();
#endif
