/*
 * main.c
 * Kernel entry point from bootloader
 * */

#include <dreamcatcher.h>
#include <elf.h>
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
#include <syscalls.h>
#include <task.h>
#include <tss.h>
#include <vfs.h>
#include <vm.h>
#include <vmm.h>
#include <vmm_mapping.h>
#include <userspace.h>
#include <utils.h>
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
struct multiboot_tag_module *loaded_module = NULL;
struct multiboot_tag *tag_start = NULL;

uint64_t elf_module_start_hh = 0;

uintptr_t higherHalfDirectMapBase;

const char *multiboot_names[] = {
    "Multiboot End",
    "Boot command line",
    "Boot loader name",
    "Modules",
    "Basic Memory Information",
    "Bios Boot Device",
    "Memory Map",
    "VBE Info",
    "Framebuffer Info",
    "EFI amd64 entry address tag of Multiboot2 header",
    "APM Table",
    "EFI 32-bit system table pointer",
    "EFI 64-bit system table pointer",
    "SMBIOS tables",
    "ACPI Old RSDP",
    "ACPI New RSDP",
    "Networking information",
    "EFI memory map",
    "EFI boot services not terminated",
    "EFI 32-bit image handle pointer",
    "EFI 64-bit image handle pointer",
    "Image load base physical address",
    "Image load base physical address"
};

void _init_basic_system(unsigned long addr){
    struct multiboot_tag* tag;
    uint32_t mbi_size = *(uint32_t *) (addr + _HIGHER_HALF_KERNEL_MEM_START);
    uint64_t end_of_mapped_physical_memory = end_of_mapped_memory - _HIGHER_HALF_KERNEL_MEM_START;
    pretty_logf(Verbose, " Addr: 0x%x - Size: 0x%x end_of_mapped_memory: 0x%x - physical: 0x%x", addr, mbi_size, end_of_mapped_memory, end_of_mapped_physical_memory);
    pretty_log(Info, "Initialize base system");
    //These data structure are initialized during the boot process.
    tagmem  = (struct multiboot_tag_basic_meminfo *)(multiboot_basic_meminfo + _HIGHER_HALF_KERNEL_MEM_START);
    tagmmap = (struct multiboot_tag_mmap *) (multiboot_mmap_data + _HIGHER_HALF_KERNEL_MEM_START);
    tagfb   = (struct multiboot_tag_framebuffer *) (multiboot_framebuffer_data + _HIGHER_HALF_KERNEL_MEM_START);
    //Print basic mem Info data
    pretty_logf(Info, "Available memory: lower (in kb): %d - upper (in kb): %d (hex): 0x%x - mbi_size: 0x%x", tagmem->mem_lower, tagmem->mem_upper, tagmem->mem_upper, mbi_size);
    //This size is temporary, it contains the memory avaiable up to the first hole in the memory map.
    memory_size_in_bytes = (tagmem->mem_upper + 1024) * 1024;
    //Print mmap_info
    pretty_logf(Verbose, "Memory map Size: 0x%x, Entry size: 0x%x, EntryVersion: 0x%x - size_of mmap struct: 0x%x", tagmmap->size, tagmmap->entry_size, tagmmap->entry_version, sizeof(*tagmmap));
    tag_start = (struct multiboot_tag *) (addr + _HIGHER_HALF_KERNEL_MEM_START + 8);
    _mmap_parse(tagmmap);
    pmm_setup(addr, mbi_size);
     kernel_settings.kernel_uptime = 0;
    kernel_settings.paging.page_root_address = p4_table;
    uint64_t p4_table_phys_address = (uint64_t) p4_table - _HIGHER_HALF_KERNEL_MEM_START;
    kernel_settings.paging.hhdm_page_root_address = (uint64_t*) hhdm_get_variable( (uintptr_t) p4_table_phys_address);
    //pretty_logf(Verbose, "p4_table[510]: %x - ADDRESS: %x", p4_table[510], kernel_settings.paging.hhdm_page_root_address[510]);
    vmm_init(VMM_LEVEL_SUPERVISOR, NULL);

    //Print framebuffer info
    pretty_logf(Verbose, "Framebuffer info: (type: 0x%x) Address: 0x%x", tagfb->common.framebuffer_type, tagfb->common.framebuffer_addr);
    pretty_logf(Verbose, "width: 0x%d - height: 0x%d - bpp: 0x%x - pitch: 0x%x", tagfb->common.framebuffer_width, tagfb->common.framebuffer_height, tagfb->common.framebuffer_bpp, tagfb->common.framebuffer_pitch);
    set_fb_data(tagfb);

    uint32_t pw, ph, cw, ch;
    get_framebuffer_mode(&pw, &ph, &cw, &ch);

    framebuffer_data.number_of_rows = cw;
    framebuffer_data.number_of_lines = ch;
    pretty_logf(Verbose, "Number of lines: %d", framebuffer_data.number_of_lines);

    pretty_logf(Verbose, "Total framebuffer size is: 0x%x", framebuffer_data.memory_size);

    tagacpi = (struct multiboot_tag *) (multiboot_acpi_info + _HIGHER_HALF_KERNEL_MEM_START);
    if(tagacpi->type == MULTIBOOT_TAG_TYPE_ACPI_OLD){
        tagold_acpi = (struct multiboot_tag_old_acpi *)tagacpi;
        pretty_logf(Info, "Found acpi RSDP: %x - Address: 0x%x", tagold_acpi->type, (unsigned long) &tagold_acpi);
        RSDPDescriptor *descriptor = (RSDPDescriptor *)(tagacpi+1);
        parse_SDT((uint64_t) descriptor, MULTIBOOT_TAG_TYPE_ACPI_OLD);
        validate_SDT((char *) descriptor, sizeof(RSDPDescriptor));
    } else if(tagacpi->type == MULTIBOOT_TAG_TYPE_ACPI_NEW){
        tagnew_acpi = (struct multiboot_tag_new_acpi *)tagacpi;
        pretty_logf(Info, "Found acpi RSDPv2: %x - Address: 0x%x", tagnew_acpi->type, (unsigned long) &tagnew_acpi);
        RSDPDescriptor20 *descriptor = (RSDPDescriptor20 *) (tagacpi+1);
        parse_SDT((uint64_t) descriptor, MULTIBOOT_TAG_TYPE_ACPI_NEW);
        validate_SDT((char *) descriptor, sizeof(RSDPDescriptor20));
    }
    tag_start = (struct multiboot_tag *) (addr + _HIGHER_HALF_KERNEL_MEM_START + 8);
    pretty_logf(Verbose, " Tag start: 0x%x", tag_start);
    for (tag=(struct multiboot_tag *) (addr + _HIGHER_HALF_KERNEL_MEM_START + 8);
        tag->type != MULTIBOOT_TAG_TYPE_END;
        tag = (struct multiboot_tag *) ((multiboot_uint8_t *) tag + ((tag->size + 7) & ~7))){
        switch(tag->type){
            case MULTIBOOT_TAG_TYPE_MODULE:
                loaded_module = (struct multiboot_tag_module *) tag;
                pretty_logf(Verbose, " \t[Tag 0x%x] (%s): Size: 0x%x - mod_start: 0x%x : mod_end: 0x%x" , loaded_module->type, multiboot_names[loaded_module->type], loaded_module->size, loaded_module->mod_start, loaded_module->mod_end);
                break;
            default:
                pretty_logf(Verbose, "\t[Tag 0x%x] (%s): Size: 0x%x",  tag->type, multiboot_names[tag->type], tag->size);
                break;
        }
    }
}

void kernel_start(unsigned long addr, unsigned long magic){
    uint64_t elf_module_start_phys  = 0;
    uint64_t elf_module_start_hh = 0;
    qemu_init_debug();
    psf_font_version = _psf_get_version(_binary_fonts_default_psf_start);
    init_idt();
    load_idt();
    init_log(LOG_OUTPUT_SERIAL, Verbose, false);
    #if USE_FRAMEBUFFER == 1
    uint8_t psf_type = _psf_get_version(_binary_fonts_default_psf_start);
    pretty_log(Info, "Welcome to:");
    pretty_log(Info, "\t____                    ______ ______");
    pretty_log(Info, "\t|    \\ ___ ___ ___ _____|     |   __|");
    pretty_log(Info, "\t|  |  |  _| -_| = |     |  |  |__   |");
    pretty_log(Info, "\t|____/|_| |___|__||_|_|_|_____|_____|");
    pretty_logf(Info, "PSF v%d found", psf_type);

    if(psf_type == 1){
        PSFv1_Font *font = (PSFv1_Font*)_binary_fonts_default_psf_start;
        pretty_logf(Verbose, "\tMagic: [%x %x] - Flags: 0x%x - Charsize: 0x%x", font->magic[1], font->magic[0], font->mode, font->charsize);
    }  else {
        PSF_font *font = (PSF_font*)&_binary_fonts_default_psf_start;
        pretty_logf(Verbose, "\tVersion: [0x%x] - Magic: [0x%x] - Header size: 0x%x - flags: 0x%x", font->version, font->magic, font->headersize, font->flags);
        pretty_logf(Verbose, "\tNumber of glyphs: [0x%x] - Bytes per glyphs: [0x%x]", font->numglyph, font->bytesperglyph);
        pretty_logf(Verbose, "\tWidth: [0x%x] - Height: [0x%x]", font->width, font->height);
    }
    higherHalfDirectMapBase = ((uint64_t) HIGHER_HALF_ADDRESS_OFFSET + VM_KERNEL_MEMORY_PADDING);
    pretty_logf(Verbose, "HigherHalf Initial entries: pml4: %d, pdpr: %d, pd: %d", PML4_ENTRY((uint64_t) higherHalfDirectMapBase), PDPR_ENTRY((uint64_t) higherHalfDirectMapBase), PD_ENTRY((uint64_t) higherHalfDirectMapBase));
    pretty_logf(Verbose, "Using page size: 0x%x" , PAGE_SIZE_IN_BYTES);

    pretty_logf(Verbose, "Kernel End: 0x%x - Physical: 0x%x", (unsigned long)&_kernel_end, (unsigned long)&_kernel_physical_end);
    _init_basic_system(addr);
    // Reminder here: The first 8 bytes have a fixed structure in the multiboot info:
    // They are: 0-4: size of the boot information in bytes
    //           4-8: Reserved (0)
    unsigned size = *(unsigned*)(addr + _HIGHER_HALF_KERNEL_MEM_START);

    if(magic != 0x36d76289){
        pretty_log(Fatal, "Failed to verify magic number. Something is wrong");
    }

    _fb_printStr("Ciao!",  0x000000, 0xEB72F9);
    _fb_printStr(" -- Welcome --", 0x000000, 0x00FF80);
    _fb_printStr(" -- Benvenuti --", 0x000000, 0xFFFFFF);
    _fb_printStr(" -- Bienvenidos --", 0x000000, 0xFF0000);
    _fb_printStr("\tDreamos64",  0xFFFFFF, 0x3333ff);
    _fb_printStr("Thanks\n\tfor\n using it", 0xFFFFFF, 0x3333ff);
    //framebuffer_data.width - width
    draw_logo(framebuffer_data.width - width, 0);
#endif
    _syscalls_init();
    //_sc_putc('c', 0);
    //asm("int $0x80");


    initialize_kheap();
    kernel_settings.paging.page_generation = 0;
    init_apic();
    if (loaded_module != NULL) {
        if ( load_module_hh(loaded_module) ) {
            pretty_log(Verbose, " The ELF module can be loaded succesfully" );
            elf_module_start_phys = loaded_module->mod_start;
        }
    }
    //The table containing the IOAPIC information is called MADT
    MADT* madt_table = (MADT*) get_SDT_item(MADT_ID);
    pretty_logf(Verbose, "MADT SIGNATURE: %x - ADDRESS: %.4s", madt_table->header.Signature, madt_table);
    pretty_logf(Verbose, "MADT local apic base: %x - Madt Length: %d", madt_table->local_apic_base, madt_table->header.Length);
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
    pretty_logf(Verbose, "Calibrated apic value: %u", apic_ticks);
    pretty_logf(Verbose, "(END of Mapped memory: 0x%x)", end_of_mapped_memory);
    vfs_init();
    uint64_t unix_timestamp = read_rtc_time();

    #if USE_FRAMEBUFFER == 1
    _fb_printStrAndNumberAt("Epoch time: ", unix_timestamp, 0, 11, 0xf5c4f1, 0x000000);
    #endif
    init_scheduler();
    char a = 'a';
    task_t* idle_task = create_task("idle", idle, &a, true);
    idle_thread = idle_task->threads;
    task_t* userspace_task = create_task("userspace_idle", NULL, &a, false);
    //create_thread("ledi", noop2, &c, eldi_task);
    //create_task("sleeper", noop3, &d);
    //execute_runtime_tests();
    start_apic_timer(kernel_settings.apic_timer.timer_ticks_base, APIC_TIMER_SET_PERIODIC, kernel_settings.apic_timer.timer_divisor);
    pretty_logf(Verbose, "(END of Mapped memory: 0x%x)", end_of_mapped_memory);
    pretty_logf(Info, "init_basic_system: Memory lower (in kb): %d - upper (in kb): %d", tagmem->mem_lower, tagmem->mem_upper);
    struct multiboot_tag_basic_meminfo *virt_phys_addr = (struct multiboot_tag_basic_meminfo *) hhdm_get_variable( (size_t) multiboot_basic_meminfo );
    pretty_log(Info, "Init end!! Starting infinite loop");

    while(1);
}
