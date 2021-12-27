#ifndef _RSDT_H_
#define _RSDT_H_

#include <acpi.h> 
#include <stdint.h>

#define MADT "APIC"

typedef struct RSDT {
    ACPISDTHeader header;
    uint32_t tables[];
} RSDT;


typedef struct MADT {
    ACPISDTHeader header;
    uint32_t local_apic_base;
    uint32_t flags;
} MADT;


extern uint32_t rsdtTablesTotal;
extern RSDT* rsdt_root;
#endif
