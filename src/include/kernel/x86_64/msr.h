#ifndef __MSR_H
#define __MSR_H

#include <stdint.h>

#define IA32_APIC_BASE 0x1b

uint64_t rdmsr(uint32_t address);
void wrmsr(uint32_t address, uint64_t value);
#endif
