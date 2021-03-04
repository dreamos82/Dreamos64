#include <idt.h>
#include <video.h>
#include <kernel/qemu.h>
#include <stdint.h>

static IDT_descriptor idt_table[IDT_SIZE];

void int_14(){
	qemu_write_string("Page fault called\n");
	qemu_write_string("Looping...");
	/*uint64_t test = 25;
	char number[30] = "";
	_getHexString(number, test);*/
	//qemu_write_string(number);
	while(1);
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
	/*set_idt_entry(0x00, IDT_PRESENT_FLAG | IDT_INTERRUPT_TYPE_FLAG, 0x08, 0,int_14);
	set_idt_entry(0x01, IDT_PRESENT_FLAG | IDT_INTERRUPT_TYPE_FLAG, 0x08, 0,int_14);
	set_idt_entry(0x02, IDT_PRESENT_FLAG | IDT_INTERRUPT_TYPE_FLAG, 0x08, 0,int_14);
	set_idt_entry(0x03, IDT_PRESENT_FLAG | IDT_INTERRUPT_TYPE_FLAG, 0x08, 0,int_14);
	set_idt_entry(0x04, IDT_PRESENT_FLAG | IDT_INTERRUPT_TYPE_FLAG, 0x08, 0,int_14);
	set_idt_entry(0x05, IDT_PRESENT_FLAG | IDT_INTERRUPT_TYPE_FLAG, 0x08, 0,int_14);
	set_idt_entry(0x06, IDT_PRESENT_FLAG | IDT_INTERRUPT_TYPE_FLAG, 0x08, 0,int_14);
	set_idt_entry(0x07, IDT_PRESENT_FLAG | IDT_INTERRUPT_TYPE_FLAG, 0x08, 0,int_14);
	set_idt_entry(0x08, IDT_PRESENT_FLAG | IDT_INTERRUPT_TYPE_FLAG, 0x08, 0,int_14);
	set_idt_entry(0x09, IDT_PRESENT_FLAG | IDT_INTERRUPT_TYPE_FLAG, 0x08, 0,int_14);
	set_idt_entry(0x0A, IDT_PRESENT_FLAG | IDT_INTERRUPT_TYPE_FLAG, 0x08, 0,int_14);
	set_idt_entry(0x0B, IDT_PRESENT_FLAG | IDT_INTERRUPT_TYPE_FLAG, 0x08, 0,int_14);
	set_idt_entry(0x0C, IDT_PRESENT_FLAG | IDT_INTERRUPT_TYPE_FLAG, 0x08, 0,int_14);
	set_idt_entry(0x0D, IDT_PRESENT_FLAG | IDT_INTERRUPT_TYPE_FLAG, 0x08, 0,int_14);*/
	set_idt_entry(0x0E, IDT_PRESENT_FLAG | IDT_INTERRUPT_TYPE_FLAG, 0x08, 0, interrupt_service_routine_14);
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
