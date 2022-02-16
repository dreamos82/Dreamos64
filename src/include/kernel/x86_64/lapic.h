#ifndef _LAPIC_H
#define _LAPIC_H

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
#define APIC_ID_REGISTER_OFFSET 0x20



#define MASTER_PIC_DATA_PORT 0x21
#define SLAVE_PIC_DATA_PORT 0xA1



void init_apic();
void init_local_vector_table();

void start_apic_timer(uint16_t, bool);
void write_apic_register(uint32_t, uint32_t);
void write_apic_icr_register(uint64_t);

uint32_t read_apic_register(uint32_t);

uint32_t lapic_id();
bool lapic_is_x2();

void disable_pic();

#endif
