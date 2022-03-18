#ifndef _MADT_H
#define _MADT_H

#include <stdint.h>
#include <rsdt.h>

#define MADT_PROCESSOR_LOCAL_APIC    0
#define MADT_IO_APIC 1
#define MADT_IO_APIC_INTERRUPT_SOURCE_OVERRIDE 2
#define MADT_NMI_INTERRUPT_SOURCE    3
#define MADT_LOCAL_APIC_NMI  4
#define MADT_LOCAL_APIC_ADDRESS_OVERRIDE 5
#define MADT_PRORCESSOR_LOCAL_X2APIC 9

typedef struct MADT_Item {
    uint8_t type;
    uint8_t length;
} __attribute__((packed)) MADT_Item;

typedef struct IO_APIC {
    uint8_t ioapic_id;
    uint8_t reserved;
    uint32_t address;
    uint32_t global_system_interrupt_base;
} __attribute__((packed)) IO_APIC_Item;

typedef struct IO_APIC_source_override_item_t {
    uint8_t bus_source;
    uint8_t irq_source;
    uint32_t    global_system_interrupt;
    uint16_t    flags;
} __attribute__((packed)) IO_APIC_source_override_item_t;

MADT_Item* get_MADT_item(MADT*, uint8_t, uint8_t);
void print_madt_table(MADT*);
#endif
