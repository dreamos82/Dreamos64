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
#include <psf.h>
#include <framebuffer.h>
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
#include <timer.h>
#include <kernel.h>
#include <string.h>
#include <scheduler.h>
#include <thread.h>
#include <rtc.h>

extern uint32_t FRAMEBUFFER_MEMORY_SIZE;
extern uint64_t multiboot_framebuffer_data;
extern uint64_t multiboot_mmap_data;
extern uint64_t multiboot_basic_meminfo;
extern uint64_t multiboot_acpi_info;
extern uint64_t end_of_mapped_memory;
extern uint8_t psf_font_version;
extern struct framebuffer_info framebuffer_data;
struct multiboot_tag_framebuffer *tagfb = NULL;
struct multiboot_tag_basic_meminfo *tagmem = NULL;
struct multiboot_tag_old_acpi *tagold_acpi = NULL;
struct multiboot_tag_new_acpi *tagnew_acpi = NULL;
struct multiboot_tag_mmap *tagmmap = NULL;
struct multiboot_tag *tagacpi = NULL;
uint64_t memory_size_in_bytes;

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
    printf("Memory map entry: 0x%x\n",  tagmmap->type);
    printf("---Size: 0x%x\n", tagmmap->size);
    printf("---Entrysize: 0x%x\n", tagmmap->entry_size);
    printf("---EntryVersion: 0x%x\n", tagmmap->entry_version);
    _mmap_parse(tagmmap);
    pmm_setup(addr, mbi_size);

    //Print framebuffer info
    printf("---framebuffer-type: 0x%x - address: 0x%x\n", tagfb->common.framebuffer_type, tagfb->common.framebuffer_addr);
    printf("---framebuffer-width: 0x%x - height: 0x%x\n", tagfb->common.framebuffer_width, tagfb->common.framebuffer_height);
    printf("---framebuffer-bpp: 0x%x\n", tagfb->common.framebuffer_bpp);
    printf("---framebuffer-pitch: 0x%x\n", tagfb->common.framebuffer_pitch);
    printf("---Virtual Address: 0x%x\n", tagfb + _HIGHER_HALF_KERNEL_MEM_START);
    set_fb_data(tagfb);
    printf("---Total framebuffer size is: 0x%x\n", framebuffer_data.memory_size);
    
    tagacpi = (struct multiboot_tag *) (multiboot_acpi_info + _HIGHER_HALF_KERNEL_MEM_START);
    if(tagacpi->type == MULTIBOOT_TAG_TYPE_ACPI_OLD){
        tagold_acpi = (struct multiboot_tag_old_acpi *)tagacpi;
        printf("Found acpi RSDP: %x\n", tagold_acpi->type);
        printf("Found acpi RSDP address: 0x%x\n", (unsigned long) &tagold_acpi);
        RSDPDescriptor *descriptor = (RSDPDescriptor *)(tagacpi+1);
        parse_SDT((uint64_t) descriptor, MULTIBOOT_TAG_TYPE_ACPI_OLD);
        validate_SDT((char *) descriptor, sizeof(RSDPDescriptor));
    } else if(tagacpi->type == MULTIBOOT_TAG_TYPE_ACPI_NEW){
        tagnew_acpi = (struct multiboot_tag_new_acpi *)tagacpi;
        printf("Found acpi RSDPv2: %x\n", tagnew_acpi->type);
        printf("Found acpi RSDP address: 0x%x\n", (unsigned long) &tagnew_acpi);
        RSDPDescriptor20 *descriptor = (RSDPDescriptor20 *) (tagacpi+1);
        parse_SDT((uint64_t) descriptor, MULTIBOOT_TAG_TYPE_ACPI_NEW);
        validate_SDT((char *) descriptor, sizeof(RSDPDescriptor20));
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
    printf("End of read configuration\n");
}

void kernel_start(unsigned long addr, unsigned long magic){
    qemu_init_debug();
    psf_font_version = get_PSF_version(_binary_fonts_default_psf_start);
    init_idt();
    load_idt();
    init_log(LOG_OUTPUT_SERIAL, Verbose, false);
    _init_basic_system(addr);
    printf("Kernel End: 0x%x - Physical: %x\n", (unsigned long)&_kernel_end, (unsigned long)&_kernel_physical_end);
    // Reminder here: The firt 8 bytes have a fixed structure in the multiboot info:
    // They are: 0-4: size of the boot information in bytes
    //           4-8: Reserved (0) 
    unsigned size = *(unsigned*)addr;
    printf("Size:  %x\n", size);
	printf("Magic: %x\n\n ", magic);
	if(magic == 0x36d76289){
        logline(Verbose, "Magic number verified\n");
	} else {
		logline(Verbose, "Failed to verify magic number. Something is wrong\n");
	}
    #if USE_FRAMEBUFFER == 1 
        if(get_PSF_version(_binary_fonts_default_psf_start) == 1){
            qemu_write_string("PSF v1 found\n");
            PSFv1_Font *font = (PSFv1_Font*)_binary_fonts_default_psf_start;
            printf("Magic: [%x %x]\n", font->magic[1], font->magic[0]);
            printf("Flags: 0x%x\n", font->mode);
            printf("Charsize: 0x%x\n", font->charsize);
        }  else {
            PSF_font *font = (PSF_font*)&_binary_fonts_default_psf_start;
            qemu_write_string("PSF v2 found\n");
            printf("Magic: 0x%x\n", font->magic);
            printf("Number of glyphs: 0x%x\n", font->numglyph);
            printf("Header size: 0x%x\n", font->headersize);
            printf("Bytes per glyphs: 0x%x\n", font->bytesperglyph);
            printf("Flags: 0x%x\n", font->flags);
            printf("Version: 0x%x\n", font->version);
            printf("Width: 0x%x\n", font->width);
            printf("Height: 0x%x\n", font->height);
            printf("Get Width test: %x\n", get_width(psf_font_version));
            printf("Get Height test: %x\n", get_height(psf_font_version));
        }
        printf("PSF stored version: %d\n", psf_font_version);
        uint32_t pw, ph, cw, ch;
        get_framebuffer_mode(&pw, &ph, &cw, &ch);
        printf("Number of lines: %d\n", ch);

        _fb_printStr("Ciao!", 1, 0, 0x000000, 0xFFFFFF);
        _fb_printStr("Dreamos64", 0, 1, 0xFFFFFF, 0x3333ff);
        _fb_printStr("Thanks\nfor\n using it", 0, 7, 0xFFFFFF, 0x3333ff);
        _fb_printStr(" -- Welcome --", 0, 3, 0xFFFFFF, 0x3333ff);

        draw_logo(0, 400);
    #endif
    
    char *cpuid_model = _cpuid_model();
    printf("Cpuid model: %s\n", cpuid_model);
    
    uint32_t cpu_info = 0;
    cpu_info = _cpuid_feature_apic();
    printf("Cpu info result: 0x%x\n", cpu_info);
    init_apic();
    _mmap_setup();

    initialize_kheap();
    kernel_settings.kernel_uptime = 0;
    //The table containing the IOAPIC information is called MADT    
    MADT* madt_table = (MADT*) get_SDT_item(MADT_ID);
    loglinef(Verbose, "Madt ADDRESS: %x", madt_table);
    loglinef(Verbose, "Madt SIGNATURE: %.4s", madt_table->header.Signature);
    loglinef(Verbose, "Madt Length: %d", madt_table->header.Length);
    loglinef(Verbose, "MADT local apic base: %x", madt_table->local_apic_base);
    print_madt_table(madt_table);
    init_ioapic(madt_table);
    init_keyboard();
    set_irq(KEYBOARD_IRQ, IOREDTBL1, 0x21, 0, 0, false);
    set_irq(PIT_IRQ, IOREDTBL2, 0x22, 0, 0, true);
    asm("sti");

    uint32_t apic_ticks = calibrate_apic();
    kernel_settings.apic_timer.timer_ticks_base = apic_ticks;
    loglinef(Verbose, "Calibrated apic value: %u", apic_ticks); 
    loglinef(Verbose, "(END of Mapped memory: 0x%x)", end_of_mapped_memory);
    logline(Info, "Init end!! Starting infinite loop");
    logline(Info, "Hello world, this is a test log!");
    uint64_t unix_timestamp = read_rtc_time();
    #if USE_FRAMEBUFFER == 1
    _fb_printStrAndNumber("Epoch time: ", unix_timestamp, 0, 5, 0xf5c4f1, 0x000000);
    #endif 
    loglinef(Verbose, "Epoch time: %u", unix_timestamp);
    init_scheduler();
    char a = 'a';
    char b = 'b';
    char c = 'c';
    create_thread("idle", noop,  &a);
    create_thread("eldi", noop2, &b);
    create_thread("ledi", noop2, &c);
    create_thread("sleeper", noop3, &c);
    start_apic_timer(kernel_settings.apic_timer.timer_ticks_base, APIC_TIMER_SET_PERIODIC, kernel_settings.apic_timer.timer_divisor);

    while(1);
}

