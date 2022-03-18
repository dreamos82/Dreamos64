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
#include <lapic.h>
#include <acpi.h>
#include <string.h>
#include <bitmap.h>
#include <pmm.h>
#include <mmap.h>
#include <vmm.h>
#include <kheap.h>
#include <rsdt.h>
#include <madt.h>
#include <stdio.h>
#include <numbers.h>
#include <ioapic.h>
#include <keyboard.h>
#include <logging.h>

extern char _binary_fonts_default_psf_size;
extern char _binary_fonts_default_psf_start;
extern char _binary_fonts_default_psf_end;
extern uint32_t FRAMEBUFFER_MEMORY_SIZE;
extern uint64_t multiboot_framebuffer_data;
extern uint64_t multiboot_mmap_data;
extern uint64_t multiboot_basic_meminfo;
extern uint64_t multiboot_acpi_info;
extern uint64_t end_of_mapped_memory;
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
    printf("Found basic mem Mem info type: 0x%x\n", tagmem->type);
    printf("Memory lower (in kb): %d - upper (in kb): %d\n", tagmem->mem_lower, tagmem->mem_upper);
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
        printf("Found acpi RSDP: %x\n", tagold_acpi->type);
        printf("Found acpi RSDP address: 0x%x\n", (unsigned long) &tagold_acpi);
        RSDPDescriptor *descriptor = (RSDPDescriptor *)(tagacpi+1);
        validate_RSDP((char *) descriptor, sizeof(RSDPDescriptor));
        parse_SDT((uint64_t) descriptor, MULTIBOOT_TAG_TYPE_ACPI_OLD);
    } else if(tagacpi->type == MULTIBOOT_TAG_TYPE_ACPI_NEW){
        tagnew_acpi = (struct multiboot_tag_new_acpi *)tagacpi;
        printf("Found acpi RSDPv2: %x\n", tagnew_acpi->type);
        printf("Found acpi RSDP address: 0x%x\n", (unsigned long) &tagnew_acpi);
        RSDPDescriptor20 *descriptor = (RSDPDescriptor20 *) (tagacpi+1);
        parse_RSDTv2(descriptor);
        validate_RSDP((char *) descriptor, sizeof(RSDPDescriptor20));
    }
 
	for (tag=(struct multiboot_tag *) (addr + _HIGHER_HALF_KERNEL_MEM_START + 8);
		tag->type != MULTIBOOT_TAG_TYPE_END;
		tag = (struct multiboot_tag *) ((multiboot_uint8_t *) tag 
										+ ((tag->size + 7) & ~7))){
        switch(tag->type){
            default:
                printf("--Tag 0x%x - Size: 0x%x\n", tag->type, tag->size);
                break;
        }
    }
    _printStr("End of read configuration\n");
}

void kernel_start(unsigned long addr, unsigned long magic){
    extern unsigned int _kernel_end;
    extern unsigned int _kernel_physical_end;
    qemu_init_debug();
    init_log(LOG_OUTPUT_FRAMEBUFFER, Verbose, false);
    init_idt();
    load_idt();
    _init_basic_system(addr);
    logline(Info, "Hello world, this is a test log!");
    printf("Kernel End: 0x%x - Physical: %x\n", (unsigned long)&_kernel_end, (unsigned long)&_kernel_physical_end);
    //test_image();
    // Reminder here: The firt 8 bytes have a fixed structure in the multiboot info:
    // They are: 0-4: size of the boot information in bytes
    //           4-8: Reserved (0) 
    unsigned size = *(unsigned*)addr;
    printf("Size:  %x\n", size);
	printf("Magic: %x\n\n ", magic);
	if(magic == 0x36d76289){
        printf("Magic number verified\n");
	} else {
		printf("Failed to verify magic number. Something is wrong\n");
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
        _fb_printStr("Ciao!", 1,1, 0x000000, 0xFFFFFF);
        _fb_printStr("Dreamos64", 0, 0, 0xFFFFFF, 0x3333ff);
        _fb_printStr("Thanks\nfor\n using it", 0, 6, 0xFFFFFF, 0x3333ff);
        if(get_PSF_version(&_binary_fonts_default_psf_start) == 1){
            qemu_write_string("PSF v1 found\n");
        }  else {
            qemu_write_string("PSF v2 found\n");
        }
        _fb_printStr(" -- Welcome --", 0, 2, 0xFFFFFF, 0x3333ff);
    #endif
    
    char *cpuid_model = _cpuid_model();
    printf("Cpuid model: %s\n", cpuid_model);
    
    uint32_t cpu_info = 0;
    cpu_info = _cpuid_feature_apic();
    printf("Cpu info result: 0x%x\n", cpu_info);
    init_apic();
    _mmap_setup();
    //Is that a test?
    pmm_reserve_area(0x10000, 10);

    char test_buffer[5];
    initialize_kheap();
    char test_str[8] = "hello";
    printf("test_str: %s\n", test_str);

    MADT* madt_table = (MADT*) get_RSDT_Item(MADT_ID);    
    printf("Madt ADDRESS: %x\n", madt_table);
    printf("Madt SIGNATURE: %.4s\n", madt_table->header.Signature);
    printf("Madt Length: %d\n", madt_table->header.Length);
    printf("MADT local apic base: %x\n", madt_table->local_apic_base);
    print_madt_table(madt_table);
    init_ioapic(madt_table);
    set_irq(KEYBOARD_IRQ, IOREDTBL1, 0x21, 0, 0, false);
    init_keyboard();
    set_irq(PIT_IRQ, IOREDTBL2, 0x22, 0, 0, true);
    asm("sti");

    uint32_t apic_ticks = calibrate_apic();
    printf("Calibrated apic value: %u\n", apic_ticks); 
    //set_irq(0, 0x22, 0, 0 ,0);
    //set_irq(0);
    //start_apic_timer(0, 0);
    //asm("sti");
    printf("(END of Mapped memory: 0x%x)\n", end_of_mapped_memory);
    char *a = kmalloc(5);
    printf("A: 0x%x\n", a);
    printf("Init end!! Starting infinite loop\n");
    while(1);
}

