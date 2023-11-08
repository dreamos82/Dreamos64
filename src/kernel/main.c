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
#include <hh_direct_map.h>
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
#include <fcntl.h>
#include <unistd.h>
#include <tss.h>
//#include <runtime_tests.h>

extern uint32_t FRAMEBUFFER_MEMORY_SIZE;
extern uint64_t multiboot_framebuffer_data;
extern uint64_t multiboot_mmap_data;
extern uint64_t multiboot_basic_meminfo;
extern uint64_t multiboot_acpi_info;
extern uint64_t end_of_mapped_memory;
extern uint8_t psf_font_version;
extern struct framebuffer_info framebuffer_data;
extern uint64_t p4_table[];
struct multiboot_tag_framebuffer *tagfb = NULL;
struct multiboot_tag_basic_meminfo *tagmem = NULL;
struct multiboot_tag_old_acpi *tagold_acpi = NULL;
struct multiboot_tag_new_acpi *tagnew_acpi = NULL;
struct multiboot_tag_mmap *tagmmap = NULL;
struct multiboot_tag *tagacpi = NULL;

void _init_basic_system(unsigned long addr){
    struct multiboot_tag* tag;
    uint32_t mbi_size = *(uint32_t *) (addr + _HIGHER_HALF_KERNEL_MEM_START);
    //These data structure are initialized durinig the boot process.
    tagmem  = (struct multiboot_tag_basic_meminfo *)(multiboot_basic_meminfo + _HIGHER_HALF_KERNEL_MEM_START);
    tagmmap = (struct multiboot_tag_mmap *) (multiboot_mmap_data + _HIGHER_HALF_KERNEL_MEM_START);
    tagfb   = (struct multiboot_tag_framebuffer *) (multiboot_framebuffer_data + _HIGHER_HALF_KERNEL_MEM_START);
    //Print basic mem Info data
    loglinef(Verbose, "(kernel_main) init_basic_system: Found basic mem Mem info type: 0x%x", tagmem->type);
    loglinef(Verbose, "(kernel_main) init_basic_system: Memory lower (in kb): %d - upper (in kb): %d", tagmem->mem_lower, tagmem->mem_upper);
    memory_size_in_bytes = (tagmem->mem_upper + 1024) * 1024;
    //Print mmap_info
    loglinef(Verbose, "(kernel_main) init_basic_system: Memory map entry: 0x%x",  tagmmap->type);
    loglinef(Verbose, "(kernel_main) init_basic_system: Mmap Size: 0x%x, Entry size: 0x%x, EntryVersion: 0x%x", tagmmap->size, tagmmap->entry_size, tagmmap->entry_version);
    _mmap_parse(tagmmap);
    pmm_setup(addr, mbi_size);

    //Print framebuffer info
    loglinef(Verbose, "(kernel_main) init_basic_system: Frambueffer info: (type: 0x%x) Address: 0x%x", tagfb->common.framebuffer_type, tagfb->common.framebuffer_addr);
    loglinef(Verbose, "(kernel_main) init_basic_system: width: 0x%x - height: 0x%x - bpp: 0x%x - pitch: 0x%x", tagfb->common.framebuffer_width, tagfb->common.framebuffer_height, tagfb->common.framebuffer_bpp, tagfb->common.framebuffer_pitch);
    set_fb_data(tagfb);
    loglinef(Verbose, "(kernel_main) init_basic_system: Total framebuffer size is: 0x%x", framebuffer_data.memory_size);

    tagacpi = (struct multiboot_tag *) (multiboot_acpi_info + _HIGHER_HALF_KERNEL_MEM_START);
    if(tagacpi->type == MULTIBOOT_TAG_TYPE_ACPI_OLD){
        tagold_acpi = (struct multiboot_tag_old_acpi *)tagacpi;
        loglinef(Info, "(kernel_main) init_basic_system: Found acpi RSDP: %x - Address: 0x%x", tagold_acpi->type, (unsigned long) &tagold_acpi);
        RSDPDescriptor *descriptor = (RSDPDescriptor *)(tagacpi+1);
        parse_SDT((uint64_t) descriptor, MULTIBOOT_TAG_TYPE_ACPI_OLD);
        validate_SDT((char *) descriptor, sizeof(RSDPDescriptor));
    } else if(tagacpi->type == MULTIBOOT_TAG_TYPE_ACPI_NEW){
        tagnew_acpi = (struct multiboot_tag_new_acpi *)tagacpi;
        loglinef(Info, "(kernel_main) init_basic_system: Found acpi RSDPv2: %x - Address: 0x%x", tagnew_acpi->type, (unsigned long) &tagnew_acpi);
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
                loglinef(Verbose, "(kernel_main) init_basic_system: [Tag 0x%x] Size: 0x%x", tag->type, tag->size);
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
    loglinef(Verbose, "(kernel_start): Kernel End: 0x%x - Physical: %x", (unsigned long)&_kernel_end, (unsigned long)&_kernel_physical_end);
    // Reminder here: The first 8 bytes have a fixed structure in the multiboot info:
    // They are: 0-4: size of the boot information in bytes
    //           4-8: Reserved (0)
    unsigned size = *(unsigned*)(addr + _HIGHER_HALF_KERNEL_MEM_START);

    if(magic == 0x36d76289){
        loglinef(Verbose, "(kernel_start): Magic number verified Size:  %x - Magic: %x", size, magic);
    } else {
        logline(Verbose, "(kernel_start): Failed to verify magic number. Something is wrong");
    }

#if USE_FRAMEBUFFER == 1

    if(get_PSF_version(_binary_fonts_default_psf_start) == 1){
        logline(Verbose, "(kernel_start): PSF v1 found");
        PSFv1_Font *font = (PSFv1_Font*)_binary_fonts_default_psf_start;
        loglinef(Verbose, "(kernel_start): PSF v1: Magic: [%x %x] - Flags: 0x%x - Charsize: 0x%x", font->magic[1], font->magic[0], font->mode, font->charsize);
    }  else {
        PSF_font *font = (PSF_font*)&_binary_fonts_default_psf_start;
        logline(Verbose, "(kernel_start): PSF v2 found");
        loglinef(Verbose, "(kernel_start): Version: 0x%x - Magic: 0x%x", font->version, font->magic);
        loglinef(Verbose, "(kernel_start): Number of glyphs: 0x%x - Bytes per glyphs: 0x%x", font->numglyph, font->bytesperglyph);
        loglinef(Verbose, "(kernel_start): Header size: 0x%x - flags: 0x%x", font->headersize, font->flags);
        loglinef(Verbose, "(kernel_start): Width: 0x%x - Height: 0x%x", font->width, font->height);
    }

    uint32_t pw, ph, cw, ch;
    get_framebuffer_mode(&pw, &ph, &cw, &ch);
    loglinef(Verbose, "(kernel_start): Number of lines: %d", ch);

    _fb_printStr("Ciao!", 1, 0, 0x000000, 0xFFFFFF);
    _fb_printStr("Dreamos64", 0, 1, 0xFFFFFF, 0x3333ff);
    _fb_printStr("Thanks\nfor\n using it", 0, 7, 0xFFFFFF, 0x3333ff);
    _fb_printStr(" -- Welcome --", 0, 3, 0xFFFFFF, 0x3333ff);

    draw_logo(0, 400);
#endif

    init_apic();
    _mmap_setup();
    vmm_init(VMM_LEVEL_SUPERVISOR, NULL);
    hhdm_map_physical_memory();
    initialize_kheap();
    kernel_settings.kernel_uptime = 0;
    kernel_settings.paging.page_root_address = p4_table;
    uint64_t p4_table_phys_address = (uint64_t) p4_table - _HIGHER_HALF_KERNEL_MEM_START;
    kernel_settings.paging.hhdm_page_root_address = (uint64_t*) hhdm_get_variable( (uintptr_t) p4_table_phys_address);
    loglinef(Verbose, "(kernel_main) p4_table[510]: %x - ADDRESS: %x", p4_table[510], kernel_settings.paging.hhdm_page_root_address[510]);
    kernel_settings.paging.page_generation = 0;
    //The table containing the IOAPIC information is called MADT
    MADT* madt_table = (MADT*) get_SDT_item(MADT_ID);
    loglinef(Verbose, "(kernel_main) Madt SIGNATURE: %x - ADDRESS: %.4s", madt_table->header.Signature, madt_table);
    loglinef(Verbose, "(kernel_main) MADT local apic base: %x - Madt Length: %d", madt_table->local_apic_base, madt_table->header.Length);
    print_madt_table(madt_table);
    init_ioapic(madt_table);
    init_keyboard();
    set_irq(KEYBOARD_IRQ, IOREDTBL1, 0x21, 0, 0, false);
    set_irq(PIT_IRQ, IOREDTBL2, 0x22, 0, 0, true);
    initialize_tss();
    load_tss();
    asm("sti");

    uint32_t apic_ticks = calibrate_apic();
    kernel_settings.apic_timer.timer_ticks_base = apic_ticks;
    loglinef(Verbose, "(kernel_main) Calibrated apic value: %u", apic_ticks);
    loglinef(Verbose, "(kernel_main) (END of Mapped memory: 0x%x)", end_of_mapped_memory);
    vfs_init();
    uint64_t unix_timestamp = read_rtc_time();
    #if USE_FRAMEBUFFER == 1
    _fb_printStrAndNumber("Epoch time: ", unix_timestamp, 0, 5, 0xf5c4f1, 0x000000);
    #endif
    init_scheduler();
    char a = 'a';
    char b = 'b';
    char c = 'c';
    char d = 'd';
    task_t* idle_task = create_task("idle", noop, &a);
    idle_thread = idle_task->threads;
    task_t* eldi_task = create_task("eldi", noop2, &b);
    create_thread("ledi", noop2, &c, eldi_task);
    create_task("sleeper", noop3, &d);
    print_thread_list(eldi_task->task_id);
    int fd_id = open("/home/ivan/testfile.txt", 0);
    loglinef(Verbose, "(kernel_main) Obtained fd id: %d fs_fd_id: %d", fd_id, vfs_opened_files[fd_id].fs_specific_id);
    char buffer[15] = "";
    read(fd_id, buffer, 15);
    loglinef(Verbose, "(kernel_main) Output of read: %s", buffer);
    int result = close(fd_id);
    loglinef(Verbose, "(kernel_main) Closing file with id: %d", result);
    //execute_runtime_tests();
    start_apic_timer(kernel_settings.apic_timer.timer_ticks_base, APIC_TIMER_SET_PERIODIC, kernel_settings.apic_timer.timer_divisor);
    loglinef(Verbose, "(kernel_main) (END of Mapped memory: 0x%x)", end_of_mapped_memory);
    loglinef(Verbose, "(kernel_main) init_basic_system: Memory lower (in kb): %d - upper (in kb): %d", tagmem->mem_lower, tagmem->mem_upper);
    // Testing that the hhdm is actually working
    struct multiboot_tag_basic_meminfo *virt_phys_addr = (struct multiboot_tag_basic_meminfo *) hhdm_get_variable( (size_t) multiboot_basic_meminfo );
    loglinef(Verbose, "(kernel_main) init_basic_system: Memory lower (in kb): %d - upper (in kb): %d", virt_phys_addr->mem_lower, virt_phys_addr->mem_upper);
    logline(Info, "(kernel_main) Init end!! Starting infinite loop");
    while(1);
}
