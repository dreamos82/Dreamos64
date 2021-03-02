/*
 * main.c 
 * Kernel entry point from bootloader
 * */

#include <multiboot.h>
#include <kernel/video.h>
#include <kernel/io.h>
#include <kernel/qemu.h>
#include <main.h>

struct multiboot_tag_framebuffer *tagfb;

void _read_configuration_from_multiboot(unsigned long addr){
    struct multiboot_tag* tag;
    char number[30];
	for (tag=(struct multiboot_tag *) (addr + 8);
		tag->type != MULTIBOOT_TAG_TYPE_END;
		tag = (struct multiboot_tag *) ((multiboot_uint8_t *) tag 
										+ ((tag->size + 7) & ~7))){

        switch(tag->type){
            case MULTIBOOT_TAG_TYPE_FRAMEBUFFER:
                qemu_write_string("Found Multiboot framebuffer: ");
                _getHexString(number, tag->type);
                qemu_write_string(number);
                qemu_write_string("\n");
                tagfb = (struct multiboot_tag_framebuffer *) tag;
                qemu_write_string("---framebuffer-type: ");
                _getHexString(number, tagfb->common.framebuffer_type);
                qemu_write_string(number);
                qemu_write_string("\n");
                qemu_write_string("---framebuffer-width: ");
                _getHexString(number, tagfb->common.framebuffer_width);
                qemu_write_string(number);
                qemu_write_string("\n");
                qemu_write_string("---framebuffer-height: ");
                _getHexString(number, tagfb->common.framebuffer_height);
                qemu_write_string(number);
                qemu_write_string("\n");
                qemu_write_string("---framebuffer-address: ");
                _getHexString(number, tagfb->common.framebuffer_addr);
                qemu_write_string(number);
                qemu_write_string("\n");
                qemu_write_string("---framebuffer-bpp: ");
                _getHexString(number, tagfb->common.framebuffer_bpp);
                qemu_write_string(number);
                qemu_write_string("\n");
                //set_fb_data(tagfb);
                break;
        }

    }

}
void kernel_start(unsigned long addr, unsigned long magic){
    //struct multiboot_tag *tag;
    extern unsigned int end;
    struct multiboot_header* boot_info = (struct multiboot_header*) 0x100000;
    struct multiboot_tag *tag = (struct multiboot_tag*) (addr+8);
    unsigned int log_enabled = qemu_init_debug();
    qemu_write_string("Hello qemu log\n");
    qemu_write_string("==============\n");

    _read_configuration_from_multiboot(addr);
        
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
    qemu_write_string(number);
    qemu_write_string("\n");
    val++;
    _printNewLine();
    _printStr("Flags: ");
    _printHex(number, *val);
    _printNewLine();
    val++;  
    _printStr("Header Length: ");
    _printHex(number, *val); 
    _printStr(" OMT: ");
    _printStr(number);

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
    qemu_write_string("Tag = type: ");
    _printNumber(number, tag->type, 10);
    qemu_write_string(number);
    _printStr(" - size: ");
    qemu_write_string(" - size: ");
    _printNumber(number, tag->size, 10);
    qemu_write_string(number);
    qemu_write_string("\n");
	_printNewLine();
	for (tag = (struct multiboot_tag *) (addr + 8);
		tag->type != MULTIBOOT_TAG_TYPE_END;
		tag = (struct multiboot_tag *) ((multiboot_uint8_t *) tag 
										+ ((tag->size + 7) & ~7)))
		{
			qemu_write_string("Tag 0x");
			_getHexString(number, tag->type);
			qemu_write_string(number);
			qemu_write_string("\n");
			qemu_write_string(" Size 0x");
			_getHexString(number, tag->size);
			qemu_write_string(number);
			qemu_write_string("\n");
		}

    //_printStr(" Type: ");
    //_printHex(number, boot_mem->type);
    //_printNewLine();
    //_printStr(" MemL: ");
    //_printHex(number, boot_mem->mem_lower);
    //_printNewLine();
    //_printStr(" MemU: ");
    //_printHex(number, boot_mem->mem_upper);
   

}
