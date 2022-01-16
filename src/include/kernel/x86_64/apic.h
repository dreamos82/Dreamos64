#ifndef _APIC_H
#define _APIC_H

#include <stdint.h>
#include <stdbool.h>
#include <madt.h>

#define APIC_BSP_BIT 8
#define APIC_GLOBAL_ENABLE_BIT 11
#define APIC_BASE_ADDRESS_MASK 0xFFFFF000

#define APIC_TIMER_LVT_OFFSET 0x00000320

#define APIC_TABLE_LENGTH 0x6

#define APIC_TIMER_IDT_ENTRY 0x20
#define APIC_TIMER_CONFIGURATION_OFFSET 0x3E0
#define APIC_TIMER_INITIAL_COUNT_REGISTER_OFFSET 0x380
#define APIC_TIMER_CURRENT_COUNT_REGISTER_OFFSET 0x390

#define APIC_TIMER_DIVIDER_1 0xB
#define APIC_TIMER_DIVIDER_2 0x0
#define APIC_TIMER_DIVIDER_4 0x1
#define APIC_TIMER_DIVIDER_8 0x2
#define APIC_TIMER_DIVIDER_16 0x3
#define APIC_TIMER_DIVIDER_32 0x8
#define APIC_TIMER_DIVIDER_64 0x9
#define APIC_TIMER_DIVIDER_128 0xA

#define APIC_TIMER_MODE_ONE_SHOT 0x0
#define APIC_TIMER_MODE_PERIODIC 0x20000
#define APIC_VERSION_REGISTER_OFFSET 0x30
#define APIC_EOI_REGISTER_OFFSET 0xB0

#define APIC_SPURIOUS_VECTOR_REGISTER_OFFSET 0xF0
#define APIC_SPURIOUS_INTERRUPT_IDT_ENTRY 0xFF
#define APIC_SOFTWARE_ENABLE (1 << 8)

#define IO_APIC_ID_OFFSET   0x0
#define IO_APIC_VER_OFFSET  0x1
#define IO_APIC_ARB_OFFSET  0x2
#define IO_APIC_REDTBL_START_OFFSET 0x10


#define MASTER_PIC_DATA_PORT 0x21
#define SLAVE_PIC_DATA_PORT 0xA1

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


void init_apic();
void init_local_vector_table();

void start_apic_timer(uint16_t, bool);
void write_apic_register(uint32_t, uint32_t);

uint32_t read_apic_register(uint32_t);

void init_ioapic(MADT*);
uint32_t read_io_apic_register(uint8_t);
void write_io_apic_register(uint8_t, uint32_t);
int read_io_apic_redirect(uint8_t,  io_apic_redirect_entry_t*);
int write_io_apic_redirect(uint8_t, io_apic_redirect_entry_t);
void disable_pic();

#endif
