/*
 * main.c 
 * Kernel entry point from bootloader
 * */

#include <main.h>
#include <idt.h>
#include <multiboot.h>
#include <kernel/video.h>
#include <kernel/io.h>
#include <kernel/qemu.h>
#include <kernel/framebuffer.h>
#include <kernel/psf.h>
#include <cpu.h>
#include <apic.h>
#include <acpi.h>
#include <string.h>

struct multiboot_tag_framebuffer *tagfb;
struct multiboot_tag_basic_meminfo *tagmem;
struct multiboot_tag_old_acpi *tagold_acpi;
extern char _binary_fonts_default_psf_size;
extern char _binary_fonts_default_psf_start;
extern char _binary_fonts_default_psf_end;

void _read_configuration_from_multiboot(unsigned long addr){
    struct multiboot_tag* tag;
    char number[30];
	for (tag=(struct multiboot_tag *) (addr + 8);
		tag->type != MULTIBOOT_TAG_TYPE_END;
		tag = (struct multiboot_tag *) ((multiboot_uint8_t *) tag 
										+ ((tag->size + 7) & ~7))){

        switch(tag->type){
            case MULTIBOOT_TAG_TYPE_BASIC_MEMINFO:
                qemu_write_string("Found basic Mem Info type:");
                tagmem = (struct multiboot_tag_basic_meminfo *) tag;
                _getHexString(number, tagmem->type);
                _printStr(number);
                _printNewLine();
                _getHexString(number, tagmem->mem_lower);
                _printStr("Mem lower: ");
                _printStr(number);
                _printNewLine();
                _getHexString(number, tagmem->mem_upper);
                _printStr("Mem upper: ");
                _printStr(number);
                _printNewLine();
                break;
            case MULTIBOOT_TAG_TYPE_ACPI_OLD:
                _printStr("Found acpi RSDP\n");
                tagold_acpi = (struct multiboot_tag_old_acpi *)tag;
                RSDPDescriptor *descriptor = (RSDPDescriptor *)(++tag);
                _getHexString(number, tagold_acpi->type);
                _printStr(number);
                _printNewLine();
                _getHexString(number, tagold_acpi->size);
                _printStr(number);
                _printNewLine();
                _printStr("Descriptor signature: ");
                _printStr(descriptor->Signature);
                _printNewLine();
                _printStr("Descriptor OEMID: ");
                _printStr(descriptor->OEMID);
                _printNewLine();
                _getHexString(number, descriptor->Revision);
                _printStr("Revision: ");
                _printStr(number);
                _printNewLine();
                _printStr(" ---");
                validate_RSDP(descriptor);
                parse_RSDT(descriptor);
                break;
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
                qemu_write_string("---framebuffer-pitch: ");
                _getHexString(number, tagfb->common.framebuffer_pitch);
                qemu_write_string(number);
                qemu_write_string("\n"); 
                set_fb_data(tagfb);
                break;
        }

    }

}
void kernel_start(unsigned long addr, unsigned long magic){
    //struct multiboot_tag *tag;
    extern unsigned int _kernel_end;
    _printCh('A', WHITE);
    struct multiboot_tag *tag = (struct multiboot_tag*) (addr+8);
    unsigned int log_enabled = qemu_init_debug();
    qemu_write_string("Hello qemu log\n");
    qemu_write_string("==============\n");

    init_idt();
    load_idt();
    _read_configuration_from_multiboot(addr);
    //test_image();
    
    qemu_write_string("---Ok\n");
    unsigned size = *(unsigned*)addr;
    char number[30];
    _printNewLine();
    _printStr("Size: ");
    _printNumber(number, size, 10);
    _printCh('@', WHITE);
    _printNewLine();

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
	_printHex(number, (unsigned int)&_kernel_end);
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

    PSF_font *font = (PSF_font*)&_binary_fonts_default_psf_start;
    _printStringAndNumber("Magic: ", font->magic);
    _printStringAndNumber("Number of glyphs: ", font->numglyph);
    _printStringAndNumber("Header size: ", font->headersize);
    _printStringAndNumber("Bytes per glyphs: ", font->bytesperglyph);
    _printStringAndNumber("Flags: ", font->flags);
    _printStringAndNumber("Version: ", font->version);
    _printStringAndNumber("Width: ", font->width);
    _printStringAndNumber("Height: ", font->height);
    #if USE_FRAMEBUFFER == 1 
    _fb_putchar('C', 1, 1, 0x000000, 0xFFFFFF);
    _fb_putchar('i', 2, 1, 0x000000, 0xFFFFFF);
    _fb_putchar('a', 3, 1, 0x000000, 0xFFFFFF);
    _fb_putchar('o', 4, 1, 0x000000, 0xFFFFFF);
    _fb_putchar('!', 5, 1, 0x000000, 0xFFFFFF);
    _fb_printStr("Dreamos64", 0, 0, 0xFFFFFF, 0x3333ff);
    if(get_PSF_version(&_binary_fonts_default_psf_start) == 1){
        qemu_write_string("PSF v1 found\n");
    }  else {
        qemu_write_string("PSF v2 found\n");
    }
    _printStringAndNumber("Size of psv1_font: ", sizeof(PSFv1_Font));
    #endif
    char *cpuid_model = _cpuid_model();
    _printStr(cpuid_model);
    uint32_t cpu_info = 0;
    cpu_info = _cpuid_feature_apic();
    _printHex(number, cpu_info);
    _printNewLine();
    //test_strcmp();
    init_apic();
    asm("hlt");
}
