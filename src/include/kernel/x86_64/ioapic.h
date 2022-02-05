#ifndef __IO_APIC_H
#define __IO_APIC_H

#include <madt.h> 
#include <stdbool.h>

#define IO_APIC_ID_OFFSET   0x0
#define IO_APIC_VER_OFFSET  0x1
#define IO_APIC_ARB_OFFSET  0x2
#define IO_APIC_REDTBL_START_OFFSET 0x10

#define IO_APIC_SOURCE_OVERRIDE_MAX_ITEMS   0x10

#define IO_APIC_DELIVERY_MODE_FIXED 0x0
#define IO_APIC_DESTINATION_MODE_PHYSICAL 0x0
#define IO_APIC_INTERRUPT_PIN_POLARITY_HIGH_ACTIVE  0x0
#define IO_APIC_TRIGGER_MODE_EDGE   0x0

#define IO_APIC_POLARITY_BIT_MASK 0x2000
#define IO_APIC_TRIGGER_MODE_BIT_MASK 0x8000
#define TIMER_IRQ 0x00
#define KEYBOARD_IRQ 0x01
#define PIT_IRQ 0x02

#define IOREDTBL0   0x10
#define IOREDTBL1   0x12
#define IOREDTBL2   0x14
#define IOREDTBL3   0x16
#define IOREDTBL4   0x18
#define IOREDTBL5   0x1A
#define IOREDTBL6   0x1C
#define IOREDTBL7   0x1E
#define IOREDTBL8   0x20
#define IOREDTBL9   0x22
#define IOREDTBL10  0x24
#define IOREDTBL11  0x26
#define IOREDTBL12  0x28
#define IOREDTBL13  0x2A
#define IOREDTBL14  0x2C
#define IOREDTBL15  0x2E
#define IOREDTBL16  0x30
#define IOREDTBL17  0x32
#define IOREDTBL18  0x34
#define IOREDTBL19  0x36
#define IOREDTBL20  0x38
#define IOREDTBL21  0x3A
#define IOREDTBL22  0x3C
#define IOREDTBL23  0x3E

typedef union io_apic_redirect_entry_t {
    struct
    {
    uint64_t    vector  :8;
    uint64_t    delivery_mode   :3;
    uint64_t    destination_mode    :1;
    uint64_t    delivery_status :1;
    uint64_t    pin_polarity    :1;
    uint64_t    remote_irr  :1;
    uint64_t    trigger_mode    :1;
    uint64_t    interrupt_mask  :1;
    uint64_t    reserved    :39;
    uint64_t    destination_field   :8;
    };
    uint64_t raw;
} __attribute__((packed)) io_apic_redirect_entry_t;

void init_ioapic(MADT*);
uint32_t read_io_apic_register(uint8_t);
void write_io_apic_register(uint8_t, uint32_t);
int read_io_apic_redirect(uint8_t,  io_apic_redirect_entry_t*);
int write_io_apic_redirect(uint8_t, io_apic_redirect_entry_t);

void set_irq(uint8_t, uint8_t, uint8_t, uint8_t, uint32_t, bool);
int set_irq_mask(uint8_t, bool);
int parse_io_apic_interrupt_source_overrides(MADT*);
#endif
