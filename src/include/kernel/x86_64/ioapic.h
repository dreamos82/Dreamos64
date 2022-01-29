#ifndef __IO_APIC_H
#define __IO_APIC_H

#include <madt.h> 

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

void set_irq(uint8_t, uint8_t, uint8_t, uint32_t);

#endif
