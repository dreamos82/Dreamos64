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
#include <vmm.h>
#include <kheap.h>

extern char _binary_fonts_default_psf_size;
extern char _binary_fonts_default_psf_start;
extern char _binary_fonts_default_psf_end;
extern uint32_t FRAMEBUFFER_MEMORY_SIZE;
extern uint64_t multiboot_framebuffer_data;
extern uint64_t multiboot_mmap_data;
extern uint64_t multiboot_basic_meminfo;
extern uint64_t multiboot_acpi_info;
struct multiboot_tag_framebuffer *tagfb = NULL;
struct multiboot_tag_basic_meminfo *tagmem = NULL;
struct multiboot_tag_old_acpi *tagold_acpi = NULL;
struct multiboot_tag_new_acpi *tagnew_acpi = NULL;
struct multiboot_tag_mmap *tagmmap = NULL;
struct multiboot_tag *tagacpi = NULL;
uint32_t memory_size_in_bytes;

void _init_basic_system(unsigned long addr){
    struct multiboot_tag* tag;
    uint32_t mbi_size = *(uint32_t *) addr;
    //These data structure are initialized durinig the boot process.
    tagmem  = (struct multiboot_tag_basic_meminfo *)(multiboot_basic_meminfo + _HIGHER_HALF_KERNEL_MEM_START);
    tagmmap = (struct multiboot_tag_mmap *) (multiboot_mmap_data + _HIGHER_HALF_KERNEL_MEM_START);
    tagfb   = (struct multiboot_tag_framebuffer *) (multiboot_framebuffer_data + _HIGHER_HALF_KERNEL_MEM_START);
    //Print basic mem Info data
    _printStringAndNumber("Found basic mem Mem info type: ", tagmem->type);
    _printStringAndNumber("Memory lower (in kb): ", tagmem->mem_lower);
    _printStringAndNumber("Memory upper (in kb): ", tagmem->mem_upper); 
    memory_size_in_bytes = (tagmem->mem_upper + 1024) * 1024;
    //Print mmap_info
    _printStringAndNumber("Memory map entry: ", tagmmap->type);
    _printStringAndNumber("---Size: ", tagmmap->size);
    _printStringAndNumber("---Entrysize: ", tagmmap->entry_size);
    _printStringAndNumber("---EntryVersion: ", tagmmap->entry_version);
    _printStringAndNumber("---Struct size: ", sizeof(struct multiboot_tag_mmap));
    _printStringAndNumber("---framebuffer-type: ", tagfb->common.framebuffer_type);
    _mmap_parse(tagmmap);
    _printStringAndNumber("---framebuffer-address: ", tagfb->common.framebuffer_addr);
    pmm_setup(addr, mbi_size);

    //Print framebuffer info
    _printStringAndNumber("Found multiboot framebuffer: ", tagmem->type); 
    _printStringAndNumber("---framebuffer-type: ", tagfb->common.framebuffer_type);
    _printStringAndNumber("---framebuffer-width: ", tagfb->common.framebuffer_width);
    _printStringAndNumber("---framebuffer-height: ", tagfb->common.framebuffer_height);
    _printStringAndNumber("---framebuffer-address: ", tagfb->common.framebuffer_addr);
    _printStringAndNumber("---framebuffer-bpp: ", tagfb->common.framebuffer_bpp);
    _printStringAndNumber("---framebuffer-pitch: ", tagfb->common.framebuffer_pitch);
    _printStringAndNumber("---Address: 0x", tagfb + _HIGHER_HALF_KERNEL_MEM_START);
    set_fb_data(tagfb);
    map_framebuffer(tagfb);
    _printStringAndNumber("---Total framebuffer size is:  ", FRAMEBUFFER_MEMORY_SIZE);
    
    tagacpi = (struct multiboot_tag *) (multiboot_acpi_info + _HIGHER_HALF_KERNEL_MEM_START);
    if(tagacpi->type == MULTIBOOT_TAG_TYPE_ACPI_OLD){
        tagold_acpi = (struct multiboot_tag_old_acpi *)tagacpi;
        _printStringAndNumber("Found acpi RSDP: ", tagold_acpi->type);
        _printStringAndNumber("Found acpi RSDP address: ", (unsigned long) &tagold_acpi);
        RSDPDescriptor *descriptor = (RSDPDescriptor *)(tagacpi+1);
        _printStringAndNumber("Address: ", (unsigned long) &descriptor);
        printf("Signature: %.8s\n", descriptor->Signature);
        printf("OEMID: %.6s\n", descriptor->OEMID);
        validate_RSDP(descriptor);
        parse_RSDT(descriptor);
    } else if(tagacpi->type == MULTIBOOT_TAG_TYPE_ACPI_OLD){
        tagnew_acpi = (struct multiboot_tag_new_acpi *)tagacpi;
        _printStringAndNumber("Found acpi RSDP: ", tagnew_acpi->type);
        _printStringAndNumber("Found acpi RSDP address: ", (unsigned long) &tagnew_acpi);
        _printStr("To be implemented");
    }
 
	for (tag=(struct multiboot_tag *) (addr + _HIGHER_HALF_KERNEL_MEM_START + 8);
		tag->type != MULTIBOOT_TAG_TYPE_END;
		tag = (struct multiboot_tag *) ((multiboot_uint8_t *) tag 
										+ ((tag->size + 7) & ~7))){
        switch(tag->type){
           case MULTIBOOT_TAG_TYPE_ELF_SECTIONS:
                _printStr("Found elf sections");
                _printStringAndNumber("--Size: 0x", tag->size);
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
    init_idt();
    load_idt();
    _init_basic_system(addr);
    _printStringAndNumber("Kernel End: ", (unsigned long)&_kernel_end);
    _printStringAndNumber("Kernel physical end: ", (unsigned long)&_kernel_physical_end);
    //test_image();
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
        _fb_printStr("Thanks\nfor\n using it", 0, 6, 0xFFFFFF, 0x3333ff);
        if(get_PSF_version(&_binary_fonts_default_psf_start) == 1){
            qemu_write_string("PSF v1 found\n");
        }  else {
            qemu_write_string("PSF v2 found\n");
        }
        _printStringAndNumber("Size of psv1_font: ", sizeof(PSFv1_Font));
        _fb_printStr(" -- Welcome --", 0, 2, 0xFFFFFF, 0x3333ff);
    #endif
    char *cpuid_model = _cpuid_model();
    _printStr("Cpuid model: ");
    _printStr(cpuid_model);
    _printNewLine();
    uint32_t cpu_info = 0;
    cpu_info = _cpuid_feature_apic();
    _printStringAndNumber("Cpu info result: ", cpu_info);
    printf("Init apic part\n");
    init_apic();
    _mmap_setup();
    pmm_reserve_area(0x10000, 10);
    char test_buffer[5];
    int test_size = _getDecString(test_buffer, 250);
    _printStringAndNumber("Size: ", test_size);
    _printStr(test_buffer);
    printf("Testing printf: %d TER\n", 25);
    printf("Testing printf: %x TER\n", 25);
    uint64_t *test_addr = map_vaddress((void *)0x1234567800, 0);
    //uint64_t *test_addr = (uint64_t *)0x1234567800;
	_printStringAndNumber("Mapping addr: ", (uint64_t)test_addr);
	*test_addr = 42l;
    _printStringAndNumber("Tesitng value of  new mapped addr should be 42: ", *test_addr);
    /*_printStr("Try to unmap\n");
    int unmap_result = unmap_vaddress(test_addr);
    _printStringAndNumber("Output from unmap: ", unmap_result);
    _printStr("A pf should explode now...\n");
    *test_addr = 12l;
    _printStringAndNumber("Should not print ", *test_addr);*/
    initialize_kheap();
    char test_str[8] = "hello";
    printf("test_str: %s\n", test_str);
    start_apic_timer(0, 0);
    printf("Init end!! Starting infinite loop\n");
    while(1);
}

