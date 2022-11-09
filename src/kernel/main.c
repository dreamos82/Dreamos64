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
#include <spinlock.h>
#include <task.h>
#include <vfs.h>

//#include <runtime_tests.h>

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
    loglinef(Verbose, "Found basic mem Mem info type: 0x%x", tagmem->type);
    loglinef(Verbose, "Memory lower (in kb): %d - upper (in kb): %d", tagmem->mem_lower, tagmem->mem_upper);
    memory_size_in_bytes = (tagmem->mem_upper + 1024) * 1024;
    //Print mmap_info
    loglinef(Verbose, "Memory map entry: 0x%x",  tagmmap->type);
    loglinef(Verbose, "---Size: 0x%x, Entry size: 0x%x", tagmmap->size, tagmmap->entry_size);
    loglinef(Verbose,"---EntryVersion: 0x%x", tagmmap->entry_version);
    _mmap_parse(tagmmap);
    pmm_setup(addr, mbi_size);

    //Print framebuffer info
    loglinef(Verbose, "---framebuffer-type: 0x%x - address: 0x%x", tagfb->common.framebuffer_type, tagfb->common.framebuffer_addr);
    loglinef(Verbose, "---framebuffer-width: 0x%x - height: 0x%x", tagfb->common.framebuffer_width, tagfb->common.framebuffer_height);
    loglinef(Verbose, "---framebuffer-bpp: 0x%x - framebuffer-pitch: 0x%x", tagfb->common.framebuffer_bpp, tagfb->common.framebuffer_pitch);
    loglinef(Verbose, "---Virtual Address: 0x%x", tagfb + _HIGHER_HALF_KERNEL_MEM_START);
    set_fb_data(tagfb);
    loglinef(Verbose, "---Total framebuffer size is: 0x%x", framebuffer_data.memory_size);
    
    tagacpi = (struct multiboot_tag *) (multiboot_acpi_info + _HIGHER_HALF_KERNEL_MEM_START);
    if(tagacpi->type == MULTIBOOT_TAG_TYPE_ACPI_OLD){
        tagold_acpi = (struct multiboot_tag_old_acpi *)tagacpi;
        loglinef(Verbose, "Found acpi RSDP: %x", tagold_acpi->type);
        loglinef(Verbose, "Found acpi RSDP address: 0x%x", (unsigned long) &tagold_acpi);
        RSDPDescriptor *descriptor = (RSDPDescriptor *)(tagacpi+1);
        parse_SDT((uint64_t) descriptor, MULTIBOOT_TAG_TYPE_ACPI_OLD);
        validate_SDT((char *) descriptor, sizeof(RSDPDescriptor));
    } else if(tagacpi->type == MULTIBOOT_TAG_TYPE_ACPI_NEW){
        tagnew_acpi = (struct multiboot_tag_new_acpi *)tagacpi;
        loglinef(Verbose, "Found acpi RSDPv2: %x", tagnew_acpi->type);
        loglinef(Verbose, "Found acpi RSDP address: 0x%x", (unsigned long) &tagnew_acpi);
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
                loglinef(Verbose, "--Tag 0x%x - Size: 0x%x", tag->type, tag->size);
                break;
        }
    }
}

void kernel_start(unsigned long addr, unsigned long magic){
    qemu_init_debug();
    psf_font_version = get_PSF_version(_binary_fonts_default_psf_start);
    init_idt();
    load_idt();
    init_log(LOG_OUTPUT_SERIAL, Verbose, false);
    _init_basic_system(addr);
    loglinef(Verbose, "Kernel End: 0x%x - Physical: %x", (unsigned long)&_kernel_end, (unsigned long)&_kernel_physical_end);
    // Reminder here: The first 8 bytes have a fixed structure in the multiboot info:
    // They are: 0-4: size of the boot information in bytes
    //           4-8: Reserved (0) 
    unsigned size = *(unsigned*)addr;
    loglinef(Verbose, "Size:  %x - Magic: %x", size, magic);
	if(magic == 0x36d76289){
        logline(Verbose, "Magic number verified");
	} else {
		logline(Verbose, "Failed to verify magic number. Something is wrong");
	}
    #if USE_FRAMEBUFFER == 1 
        if(get_PSF_version(_binary_fonts_default_psf_start) == 1){
            logline(Verbose, "PSF v1 found");
            PSFv1_Font *font = (PSFv1_Font*)_binary_fonts_default_psf_start;
            loglinef(Verbose, "Magic: [%x %x]", font->magic[1], font->magic[0]);
            loglinef(Verbose, "Flags: 0x%x", font->mode);
            loglinef(Verbose, "Charsize: 0x%x", font->charsize);
        }  else {
            PSF_font *font = (PSF_font*)&_binary_fonts_default_psf_start;
            logline(Verbose, "PSF v2 found");
            loglinef(Verbose, "Magic: 0x%x", font->magic);
            loglinef(Verbose, "Number of glyphs: 0x%x", font->numglyph);
            loglinef(Verbose, "Header size: 0x%x", font->headersize);
            loglinef(Verbose, "Bytes per glyphs: 0x%x", font->bytesperglyph);
            loglinef(Verbose, "Flags: 0x%x", font->flags);
            loglinef(Verbose, "Version: 0x%x", font->version);
            loglinef(Verbose, "Width: 0x%x", font->width);
            loglinef(Verbose, "Height: 0x%x", font->height);
            loglinef(Verbose, "Get Width test: %x", get_width(psf_font_version));
            loglinef(Verbose, "Get Height test: %x", get_height(psf_font_version));
        }
        loglinef(Verbose, "PSF stored version: %d", psf_font_version);
        uint32_t pw, ph, cw, ch;
        get_framebuffer_mode(&pw, &ph, &cw, &ch);
        loglinef(Verbose, "Number of lines: %d", ch);

        _fb_printStr("Ciao!", 1, 0, 0x000000, 0xFFFFFF);
        _fb_printStr("Dreamos64", 0, 1, 0xFFFFFF, 0x3333ff);
        _fb_printStr("Thanks\nfor\n using it", 0, 7, 0xFFFFFF, 0x3333ff);
        _fb_printStr(" -- Welcome --", 0, 3, 0xFFFFFF, 0x3333ff);

        draw_logo(0, 400);
    #endif
    
    char *cpuid_model = _cpuid_model();
    loglinef(Verbose, "Cpuid model: %s", cpuid_model);
    
    uint32_t cpu_info = 0;
    cpu_info = _cpuid_feature_apic();
    loglinef(Verbose, "Cpu info result: 0x%x", cpu_info);
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
    vfs_init();
    logline(Info, "Init end!! Starting infinite loop");
    uint64_t unix_timestamp = read_rtc_time();
    #if USE_FRAMEBUFFER == 1
    _fb_printStrAndNumber("Epoch time: ", unix_timestamp, 0, 5, 0xf5c4f1, 0x000000);
    #endif 
    init_scheduler();
    char a = 'a';
    char b = 'b';
    char c = 'c';
    char d = 'd';
    idle_thread = create_thread("idle", noop,  &a, NULL);
    task_t* eldi_task = create_task("eldi", noop2, &b);
    create_thread("ledi", noop2, &c, eldi_task);
    create_task("sleeper", noop3, &d);
    print_thread_list(eldi_task->task_id);
    //execute_runtime_tests();
    //test_get_task();
    start_apic_timer(kernel_settings.apic_timer.timer_ticks_base, APIC_TIMER_SET_PERIODIC, kernel_settings.apic_timer.timer_divisor);
    get_mountpoint_id("/home/dreamos82");
    while(1);
}

