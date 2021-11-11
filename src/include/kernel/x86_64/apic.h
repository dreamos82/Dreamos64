#ifndef _APIC_H
#define _APIC_H

#include <stdint.h>

#define APIC_BSP_BIT 8
#define APIC_GLOBAL_ENABLE_BIT 11
#define APIC_BASE_ADDRESS_MASK 0xFFFFF000

void init_apic();

#endif
