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
#include <bitmap.h>
#include <pmm.h>
#include <mmap.h>

struct multiboot_tag_framebuffer *tagfb = NULL;
struct multiboot_tag_basic_meminfo *tagmem = NULL;
struct multiboot_tag_old_acpi *tagold_acpi = NULL;
struct multiboot_tag_mmap *tagmmap = NULL;
extern char _binary_fonts_default_psf_size;
extern char _binary_fonts_default_psf_start;
extern char _binary_fonts_default_psf_end;
extern uint32_t FRAMEBUFFER_MEMORY_SIZE;
extern uint64_t multiboot_framebuffer_data;

void _read_configuration_from_multiboot(unsigned long addr){
    struct multiboot_tag* tag;
    struct multiboot_tag* tmp_tag;
    uint32_t tag1 = *((uint32_t*) (addr + _HIGHER_HALF_KERNEL_MEM_START)); 
    tmp_tag = (struct multiboot_tag *) (addr + _HIGHER_HALF_KERNEL_MEM_START);
    _printStringAndNumber("size of tags in total: ", tag1);
    _printStringAndNumber("type of tag in total: ", tmp_tag->type);
    //_printStringAndNumber("Total size: ", tag->size);
	for (tag=(struct multiboot_tag *) (addr + _HIGHER_HALF_KERNEL_MEM_START + 8);
		tag->type != MULTIBOOT_TAG_TYPE_END;
		tag = (struct multiboot_tag *) ((multiboot_uint8_t *) tag 
										+ ((tag->size + 7) & ~7))){
        switch(tag->type){
            case MULTIBOOT_TAG_TYPE_BASIC_MEMINFO:
                tagmem = (struct multiboot_tag_basic_meminfo *) tag;
                _printStringAndNumber("Found basic mem Mem info type: ", tagmem->type);
                _printStringAndNumber("Memory lower (in kb): ", tagmem->mem_lower);
                _printStringAndNumber("Memory upper (in kb): ", tagmem->mem_upper); 
               break;
            case MULTIBOOT_TAG_TYPE_ACPI_OLD:
                tagold_acpi = (struct multiboot_tag_old_acpi *)tag;
                _printStringAndNumber("Found acpi RSDP: ", tagold_acpi->type);
                _printStringAndNumber("Found acpi RSDP address: ", (unsigned long) &tagold_acpi);
                RSDPDescriptor *descriptor = (RSDPDescriptor *)(tag+1);
                _printStringAndNumber("Address: ", (unsigned long) &descriptor);
                _printStringAndNumber("Descriptor revision: ", descriptor->Revision);
                validate_RSDP(descriptor);
                //parse_RSDT(descriptor);
                break;
            case MULTIBOOT_TAG_TYPE_FRAMEBUFFER:
                _printStringAndNumber("Found multiboot framebuffer: ", tag->type); 
                tagfb = (struct multiboot_tag_framebuffer *) tag;
                _printStringAndNumber("---framebuffer-type: ", tagfb->common.framebuffer_type);
                _printStringAndNumber("---framebuffer-width: ", tagfb->common.framebuffer_width);
                _printStringAndNumber("---framebuffer-height: ", tagfb->common.framebuffer_height);
                _printStringAndNumber("---framebuffer-address: ", tagfb->common.framebuffer_addr);
                _printStringAndNumber("---framebuffer-bpp: ", tagfb->common.framebuffer_bpp);
                _printStringAndNumber("---framebuffer-pitch: ", tagfb->common.framebuffer_pitch);
                set_fb_data(tagfb);
                _printStringAndNumber("---Total framebuffer size is:  ", FRAMEBUFFER_MEMORY_SIZE);
                break;
            case MULTIBOOT_TAG_TYPE_MMAP:
                tagmmap = (struct multiboot_tag_mmap*) tag;
                _printStringAndNumber("Memory map entry: ", tagmmap->type);
                _printStringAndNumber("---Size: ", tagmmap->size);
                _printStringAndNumber("---Entrysize: ", tagmmap->entry_size);
                _printStringAndNumber("---EntryVersion: ", tagmmap->entry_version);
                _printStringAndNumber("---Struct size: ", sizeof(struct multiboot_tag_mmap));
                _mmap_parse(tagmmap);
                break;
            case MULTIBOOT_TAG_TYPE_ELF_SECTIONS:
                _printStr("Found elf sections");
                _printNewLine();
                break;
            default:
                _printStringAndNumber("Tag 0x", tag->type);
                _printStringAndNumber("--Size: 0x", tag->size);
                break;
        }
    }
    _printStringAndNumber("Size of end tag: ", tag->size);
    _printStr("End of read configuration\n");

}
void kernel_start(unsigned long addr, unsigned long magic){
    //struct multiboot_tag *tag;
    extern unsigned int _kernel_end;
    extern unsigned int _kernel_physical_end;
    qemu_init_debug();
    qemu_write_string("Hello qemu log\n");
    qemu_write_string("==============\n");
    init_idt();
    load_idt();
    _printStringAndNumber("Kernel End: ", (unsigned long)&_kernel_end);
    _printStringAndNumber("Kernel physical end: ", (unsigned long)&_kernel_physical_end);
    _read_configuration_from_multiboot(addr);
    //test_image();
    qemu_write_string("---Ok\n");
    unsigned size = *(unsigned*)addr;
    _printStringAndNumber("Size: ", size);
    unsigned int *val = (unsigned int *) 0x100000;
    _printStringAndNumber("Magic: ", *val++);
    _printStringAndNumber("Flags: ", *val++);
    _printStringAndNumber("Header Length: ", *val++);
    _printStringAndNumber("Checksum: ", *val);
	_printStringAndNumber("Magic: ", magic);
    _printNewLine();
	if(magic == 0x36d76289){
		_printStr("Magic number verified");
	} else {
		_printStr("Failed to verify magic number. Something wrong");
	}
    _printNewLine();

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
        for(int i = 3; i<23; i++){
            _fb_printStr("Test Line", 1, i, 0x000000, 0xFFFFFF);
        }
    #endif
    char *cpuid_model = _cpuid_model();
    _printStr("Cpuid model: ");
    _printStr(cpuid_model);
    _printNewLine();
    uint32_t cpu_info = 0;
    cpu_info = _cpuid_feature_apic();
    _printStringAndNumber("Cpu info result: ", cpu_info);
    init_apic();
    pmm_setup();
    pmm_reserve_area(0x10000, 10);
    struct multiboot_tag_framebuffer *mb_fb_value = (struct multiboot_tag_framebuffer *) (multiboot_framebuffer_data + _HIGHER_HALF_KERNEL_MEM_START);
    _printStringAndNumber("Multiboot_fb_from_boot: ", multiboot_framebuffer_data);
    _printStringAndNumber("Address: ", mb_fb_value->common.framebuffer_addr);
    //struct multiboot_tag_framebuffer *multiboot_tag_fb = (struct multiboot_tag_framebuffer *) multiboot_data;
    //_printStringAndNumber("Test: ", multiboot_tag_fb->common.framebuffer_addr);
    //_printStringAndNumber("faulter: ", *((uint64_t *)0xffffffff71012342));
    asm("hlt");
}

