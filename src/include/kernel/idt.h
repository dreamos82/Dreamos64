#ifndef _IDT_H
#define _IDT_H

#define IDT_SIZE 255

#define IDT_PRESENT_FLAG 0x80
#define IDT_INTERRUPT_TYPE_FLAG 0x0E
#define IDT_SEGMENT_SELECTOR 0x08

typedef struct IDT_desc {
   unsigned short offset_low;
   unsigned short segment_selector;
   unsigned char ist; //only first 2 bits are used, the rest is 0
   unsigned char flags; //P(resent) DPL (0) TYPE
   unsigned short offset_mid;
   unsigned int offset_high;
   unsigned int reserved;
} __attribute__((__packed__))IDT_descriptor;

typedef struct IDT_reg {
    unsigned short limit;
    unsigned long offset;
}__attribute__((__packed__))IDT_register;

void init_idt();
void set_idt_entry(unsigned short, unsigned char, unsigned short, void (*)());

void default_isr();
#endif
