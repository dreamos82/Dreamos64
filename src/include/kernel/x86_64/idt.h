#ifndef _IDT_H
#define _IDT_H

#include <stdint.h>
#include <cpu.h>

#define IDT_SIZE 256

#define IDT_PRESENT_FLAG 0x80
#define IDT_INTERRUPT_TYPE_FLAG 0x0E
#define IDT_SEGMENT_SELECTOR 0x08

#define KERNEL_CS   0x8

// Define Interrupts labels 
#define DIVIDE_ERROR 0
#define DEBUG_EXC 1
#define NMI_INTERRUPT 2
#define BREAKPOINT 3
#define OVERFLOW 4
#define BOUND_RANGE_EXCEED 5
#define INVALID_OPCODE 6
#define DEV_NOT_AVL 7
#define DOUBLE_FAULT 8
#define COPROC_SEG_OVERRUN 9
#define INVALID_TSS 10
#define SEGMENT_NOT_PRESENT 11
#define STACK_SEGMENT_FAULT 12
#define GENERAL_PROTECTION 13
#define PAGE_FAULT 14
#define INT_RSV 15
#define FLOATING_POINT_ERR 16
#define ALIGNMENT_CHECK 17
#define MACHINE_CHECK 18
#define SIMD_FP_EXC 19

#define APIC_TIMER_INTERRUPT 32
#define APIC_SPURIOUS_INTERRUPT 255

#define KEYBOARD_INTERRUPT 33
#define PIT_INTERRUPT 34

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
void load_idt();
void set_idt_entry(uint16_t , uint8_t, uint16_t, uint8_t, void (*)());

cpu_status_t* interrupts_handler(cpu_status_t*);

void default_isr();

//Extern declarations
extern IDT_descriptor idt_table[IDT_SIZE];
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
extern void interrupt_service_routine_32();
extern void interrupt_service_routine_33();
extern void interrupt_service_routine_34();
extern void interrupt_service_routine_255();

#endif
