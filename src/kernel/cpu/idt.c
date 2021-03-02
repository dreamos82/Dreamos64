#include <idt.h>

IDT_descriptor idt_table[IDT_SIZE];

void init_idt(){
	set_idt_entry(0, IDT_PRESENT_FLAG | IDT_INTERRUPT_TYPE_FLAG, 0x08, 0x0);
}

void set_idt_entry(unsigned short idx, unsigned char flags, unsigned short selector,void (*handler)() ){
	idt_table[idx].flags = flags;
	idt_table[idx].segment_selector = selector;
	idt_table[idx].offset_low = (unsigned short) ((unsigned long)handler&0xFFFF);
	idt_table[idx].offset_mid = (unsigned short) ((unsigned long)handler >> 16);
	idt_table[idx].offset_high = (unsigned int)((unsigned long)handler>> 32);
}
