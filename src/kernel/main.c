/*
 * main.c 
 * Kernel entry point from bootloader
 * */

#include <multiboot.h>
#include <kernel/video.h>
#include <kernel/io.h>
#include <kernel/qemu.h>
#include <main.h>

unsigned int log_enabled;

void _parse_multiboot_headers(){

}
void kernel_start(unsigned long addr, unsigned long magic){
    //struct multiboot_tag *tag;
    extern unsigned int end;
    struct multiboot_header* boot_info = (struct multiboot_header*) 0x100000;
    struct multiboot_tag *tag = (struct multiboot_tag*) (addr+8);
    log_enabled = qemu_init_debug();
    
    //unsigned int *video_mem = (unsigned int*)0xb8020;
    //*video_mem = 0x024f;
    //_printCh('@', WHITE);
    //if(boot_info->flags == 0){
    //    _printCh('Y', GREEN);
    //} else {
    //    _printCh('N', GREEN);
    //}
    unsigned size = *(unsigned*)addr;
    char number[30];
    _printNewLine();
    _printStr("Size: ");
    _printNumber(number, size, 10);
    _printCh('@', WHITE);
    _printNewLine();
    _printStr(" Trying to write something \n Now with a new line");

    _printNumber(number, 300, 10);

    _printNewLine();
    unsigned int *val = (unsigned int *) 0x100000;
    _printStr("Magic: ");
    _printHex(number, *val);
    val++;
    _printNewLine();
    _printStr("Flags: ");
    _printHex(number, *val);
    _printNewLine();
    val++;  
    _printStr("Header Length: ");
    _printHex(number, *val); 
    val++;  
    _printNewLine();
    _printStr("Checksum: ");
    _printHex(number, *val); 
    val++;
    _printNewLine();
    _printStr(" -- ");
    _printHex(number, *val);
    val++;
    _printNewLine();
    _printStr(" -- ");
    _printHex(number, *val);
    val++;
    _printNewLine();
    _printStr(" -- ");
    _printHex(number, *val);
    val++;
    _printNewLine();
    _printStr(" -- ");
    _printHex(number, *val);
    _printNewLine();
	_printStr("End: ");
	_printHex(number, (unsigned int)&end);
    _printNewLine();
	_printStr("Magic: ");
	_printHex(number, magic);
    _printNewLine();
	if(magic == 0x36d76289){
		_printStr("YEEEEH!!!");
	} else {
		_printStr("Ok i'm fucked");
	}
    _printNewLine();
    _printStr("Tag = type: ");
    _printNumber(number, tag->type, 10);
    _printStr(" - size: ");
    _printNumber(number, tag->size, 10);
	_printNewLine();
	for (tag = (struct multiboot_tag *) (addr + 8);
		tag->type != MULTIBOOT_TAG_TYPE_END;
		tag = (struct multiboot_tag *) ((multiboot_uint8_t *) tag 
										+ ((tag->size + 7) & ~7)))
		{
			_printStr("Tag 0x");
			_printHex(number, tag->type);
			_printStr(" Size 0x");
			_printHex(number , tag->size);
			_printNewLine();
		}

    qemu_write_string("Hello qemu log\n");
    qemu_write_string("==============\n");
    //_printStr(" Type: ");
    //_printHex(number, boot_mem->type);
    //_printNewLine();
    //_printStr(" MemL: ");
    //_printHex(number, boot_mem->mem_lower);
    //_printNewLine();
    //_printStr(" MemU: ");
    //_printHex(number, boot_mem->mem_upper);
   

}
