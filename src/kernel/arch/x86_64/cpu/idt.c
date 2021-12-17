#include <idt.h>
#include <video.h>
#include <kernel/qemu.h>
#include <stdint.h>
#include <vm.h>
#include <apic.h>

IDT_descriptor idt_table[IDT_SIZE];

void interrupts_handler(cpu_status_t *status){
    switch(status->interrupt_number){
        case PAGE_FAULT:
            _printStr("---To be handled Page fault\n");
            page_fault_handler(status->error_code);
            break;
        case GENERAL_PROTECTION:
            _printStringAndNumber("#GP Error code: ", status->error_code);
            asm("hlt");
            break;
        case APIC_TIMER_INTERRUPT:
            printf("Received timer_interrupt\n");
            write_apic_register(APIC_EOI_REGISTER_OFFSET, 0x0);
            break;
        case APIC_SPURIOUS_INTERRUPT:
            printf("Spurious interrupt received\n");
            break;
        default:
            qemu_write_string((char *) exception_names[status->interrupt_number]);
            qemu_write_string("\n");
            _printStringAndNumber("Actually i don't know what to do... Better going crazy... asdfasdasdsD - Interrupt number ", status->interrupt_number);
            asm("hlt");
            break;
    }
}

void init_idt(){
    int i = 0;
    while (i < IDT_SIZE){
        idt_table[i].flags = 0;
        idt_table[i].ist = 0;
        idt_table[i].offset_high = 0;
        idt_table[i].offset_low = 0;
        idt_table[i].reserved = 0;
        idt_table[i].offset_mid = 0;
        idt_table[i].segment_selector = 0;
        i++;
    }
    set_idt_entry(0x00, IDT_PRESENT_FLAG | IDT_INTERRUPT_TYPE_FLAG, 0x08, 0, interrupt_service_routine_0);
    set_idt_entry(0x01, IDT_PRESENT_FLAG | IDT_INTERRUPT_TYPE_FLAG, 0x08, 0, interrupt_service_routine_1);
    set_idt_entry(0x02, IDT_PRESENT_FLAG | IDT_INTERRUPT_TYPE_FLAG, 0x08, 0, interrupt_service_routine_2);
    set_idt_entry(0x03, IDT_PRESENT_FLAG | IDT_INTERRUPT_TYPE_FLAG, 0x08, 0, interrupt_service_routine_3);
    set_idt_entry(0x04, IDT_PRESENT_FLAG | IDT_INTERRUPT_TYPE_FLAG, 0x08, 0, interrupt_service_routine_4);
    set_idt_entry(0x05, IDT_PRESENT_FLAG | IDT_INTERRUPT_TYPE_FLAG, 0x08, 0, interrupt_service_routine_5);
    set_idt_entry(0x06, IDT_PRESENT_FLAG | IDT_INTERRUPT_TYPE_FLAG, 0x08, 0, interrupt_service_routine_6);
    set_idt_entry(0x07, IDT_PRESENT_FLAG | IDT_INTERRUPT_TYPE_FLAG, 0x08, 0, interrupt_service_routine_7);
    set_idt_entry(0x08, IDT_PRESENT_FLAG | IDT_INTERRUPT_TYPE_FLAG, 0x08, 0, interrupt_service_routine_error_code_8);
    set_idt_entry(0x09, IDT_PRESENT_FLAG | IDT_INTERRUPT_TYPE_FLAG, 0x08, 0, interrupt_service_routine_9);
    set_idt_entry(0x0A, IDT_PRESENT_FLAG | IDT_INTERRUPT_TYPE_FLAG, 0x08, 0, interrupt_service_routine_error_code_10);
    set_idt_entry(0x0B, IDT_PRESENT_FLAG | IDT_INTERRUPT_TYPE_FLAG, 0x08, 0, interrupt_service_routine_error_code_11);
    set_idt_entry(0x0C, IDT_PRESENT_FLAG | IDT_INTERRUPT_TYPE_FLAG, 0x08, 0, interrupt_service_routine_error_code_12);
    set_idt_entry(0x0D, IDT_PRESENT_FLAG | IDT_INTERRUPT_TYPE_FLAG, 0x08, 0, interrupt_service_routine_error_code_13);
    set_idt_entry(0x0E, IDT_PRESENT_FLAG | IDT_INTERRUPT_TYPE_FLAG, 0x08, 0, interrupt_service_routine_error_code_14);
    set_idt_entry(0x0F, IDT_PRESENT_FLAG | IDT_INTERRUPT_TYPE_FLAG, 0x08, 0, interrupt_service_routine_15);
    set_idt_entry(0x10, IDT_PRESENT_FLAG | IDT_INTERRUPT_TYPE_FLAG, 0x08, 0, interrupt_service_routine_16);
    set_idt_entry(0x11, IDT_PRESENT_FLAG | IDT_INTERRUPT_TYPE_FLAG, 0x08, 0, interrupt_service_routine_error_code_17);
    set_idt_entry(0x12, IDT_PRESENT_FLAG | IDT_INTERRUPT_TYPE_FLAG, 0x08, 0, interrupt_service_routine_18);
    set_idt_entry(0x20, IDT_PRESENT_FLAG | IDT_INTERRUPT_TYPE_FLAG, 0x08, 0, interrupt_service_routine_32);
    set_idt_entry(0x255, IDT_PRESENT_FLAG | IDT_INTERRUPT_TYPE_FLAG, 0x08, 0, interrupt_service_routine_255);
}

void set_idt_entry(uint16_t idx, uint8_t flags, uint16_t selector, uint8_t ist, void (*handler)() ){
    idt_table[idx].flags = flags;
    idt_table[idx].ist = ist;
    idt_table[idx].segment_selector = selector;
    idt_table[idx].offset_low = (uint16_t) ((uint64_t)handler&0xFFFF);
    idt_table[idx].offset_mid = (uint16_t) ((uint64_t)handler >> 16);
    idt_table[idx].offset_high = (uint32_t)((uint64_t)handler>> 32);
    idt_table[idx].reserved = 0x0;
}

void load_idt(){
    IDT_register idtr;
    idtr.limit =  IDT_SIZE * sizeof(IDT_descriptor) - 1;
    idtr.offset = (uint64_t)&idt_table;

    __asm__ __volatile__("lidt %0": :"g" (idtr));
}
