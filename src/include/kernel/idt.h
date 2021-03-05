#ifndef _IDT_H
#define _IDT_H

#include <stdint.h>
#include <cpu.h>

#define IDT_SIZE 256

#define IDT_PRESENT_FLAG 0x80
#define IDT_INTERRUPT_TYPE_FLAG 0x0E
#define IDT_SEGMENT_SELECTOR 0x08


/* TODO: DEFINE of: interrupt type, interrupt ids, wrapper function */


typedef struct IDT_desc {
   uint16_t offset_low;
   uint16_t segment_selector;
   uint8_t ist; //only first 2 bits are used, the rest is 0
   uint8_t flags; //P(resent) DPL (0) TYPE
   uint16_t offset_mid;
   uint32_t offset_high;
   uint32_t reserved; //0
} __attribute__((__packed__))IDT_descriptor;

typedef struct IDT_reg {
    uint16_t limit;
    uint64_t offset;
}__attribute__((__packed__))IDT_register;

void init_idt();
void set_idt_entry(uint16_t , uint8_t, uint16_t, uint8_t, void (*)());

void interrupts_handler(cpu_status_t*);

void default_isr();
void int_14(cpu_status_t*);

extern void interrupt_service_routine_error_code_14();
extern void interrupt_service_routine_7();
#endif
