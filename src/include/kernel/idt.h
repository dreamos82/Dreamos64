#ifndef _IDT_H
#define _IDT_H

#include <stdint.h>
#include <cpu.h>

#define IDT_SIZE 256

#define IDT_PRESENT_FLAG 0x80
#define IDT_INTERRUPT_TYPE_FLAG 0x0E
#define IDT_SEGMENT_SELECTOR 0x08


/* TODO: DEFINE of: interrupt type, interrupt ids, wrapper function */

static const char *exception_names[] = {
  "Divide by Zero Error",
  "Debug",
  "Non Maskable Interrupt",
  "Breakpoint",
  "Overflow",
  "Bound Range",
  "Invalid Opcode",
  "Device Not Available",
  "Double Fault",
  "Coprocessor Segment Overrun",
  "Invalid TSS",
  "Segment Not Present",
  "Stack-Segment Fault",
  "General Protection Fault",
  "Page Fault",
  "Reserved",
  "x87 Floating-Point Exception",
  "Alignment Check",
  "Machine Check",
  "SIMD Floating-Point Exception",
  "Reserved",
  "Reserved",
  "Reserved",
  "Reserved",
  "Reserved",
  "Reserved",
  "Reserved",
  "Reserved",
  "Reserved",
  "Reserved",
  "Security Exception",
  "Reserved"
};


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

//extern void interrupt_service_routine_error_code_14();
// extern void interrupt_service_routine_7();
extern void interrupt_service_routine_0();
extern void interrupt_service_routine_1();
extern void interrupt_service_routine_2();
extern void interrupt_service_routine_3();
extern void interrupt_service_routine_4();
extern void interrupt_service_routine_5();
extern void interrupt_service_routine_6();
extern void interrupt_service_routine_7();
extern void interrupt_service_routine_error_code_8();
extern void interrupt_service_routine_9();
extern void interrupt_service_routine_error_code_10();
extern void interrupt_service_routine_error_code_11();
extern void interrupt_service_routine_error_code_12();
extern void interrupt_service_routine_error_code_13();
extern void interrupt_service_routine_error_code_14();
extern void interrupt_service_routine_15();
extern void interrupt_service_routine_16();
extern void interrupt_service_routine_error_code_17();
extern void interrupt_service_routine_18();

#endif
