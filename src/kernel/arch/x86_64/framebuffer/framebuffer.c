#include <framebuffer.h>
#include <stacktrace.h>
#include <bitmap.h>
#include <hh_direct_map.h>
#include <framebuffer.h>
#include <logging.h>
#include <numbers.h>
#include <pmm.h>
#include <psf.h>
#include <stdio.h>
#include <video.h>
#include <vm.h>
#include <vmm.h>
#include <vmm_mapping.h>

extern uint64_t p4_table[];
extern uint64_t p3_table_hh[];
extern uint64_t p2_table[];
#if SMALL_PAGES == 1
extern uint64_t pt_tables[];
#endif

/*struct framebuffer_info framebuffer_data;*/

void* map_framebuffer(framebuffer_info fbdata) {
    uint64_t address_to_map = (uint64_t) fbdata.phys_address;
    uint64_t virtual_address_start = ensure_address_in_higher_half(address_to_map, VM_TYPE_MMIO);
    uint64_t virtual_address = virtual_address_start;
    uint64_t upper_address_to_map = address_to_map + fbdata.memory_size;
    pretty_logf(Verbose, "Preparing framebuffer: phys_addr: 0x%x, virtual_address: 0x%x - Fb size: 0x%x ", address_to_map, virtual_address, fbdata.memory_size);
    while ( address_to_map < upper_address_to_map) {
        map_phys_to_virt_addr((void*)address_to_map, (void*)virtual_address, VMM_FLAGS_PRESENT | VMM_FLAGS_WRITE_ENABLE);
        address_to_map += PAGE_SIZE_IN_BYTES;
        virtual_address += PAGE_SIZE_IN_BYTES;
    }
    pretty_logf(Verbose, "Framebuffer mapping end at 0x%x - virtual end: 0x%x", address_to_map, virtual_address);
    return (void *) virtual_address_start;
}


void set_fb_data(struct multiboot_tag_framebuffer *fbtag){
    //FRAMEBUFFER_MEM = (void*)(uint64_t)fbtag->common.framebuffer_addr;
#if USE_FRAMEBUFFER == 1
    //framebuffer_data.address = (void*)(uint64_t)_FRAMEBUFFER_MEM_START;
    //framebuffer_data.address = hhdm_get_variable((uintptr_t) (fbtag->common.framebuffer_addr));
    framebuffer_data.pitch = fbtag->common.framebuffer_pitch;
    framebuffer_data.bpp = fbtag->common.framebuffer_bpp;
    framebuffer_data.memory_size = fbtag->common.framebuffer_pitch * fbtag->common.framebuffer_height;
    framebuffer_data.width = fbtag->common.framebuffer_width;
    framebuffer_data.height = fbtag->common.framebuffer_height;
    framebuffer_data.phys_address = fbtag->common.framebuffer_addr;

    number_of_lines = 0;

    map_framebuffer(framebuffer_data);
    framebuffer_data.address = (void*)map_framebuffer(framebuffer_data);
    cur_fb_line = 0;
    cur_fb_column = 0;
    framebuffer_main_window.x_orig = 0;
    framebuffer_main_window.y_orig = 0;
    framebuffer_main_window.width = framebuffer_data.width;
    framebuffer_main_window.height = framebuffer_data.height;
    logo_area_ptr = NULL;

#endif
}

