#include <bitmap.h>
#include <hh_direct_map.h>
#include <framebuffer.h>
#include <logging.h>
#include <numbers.h>
#include <pmm.h>
#include <psf.h>
#include <stdio.h>
#include <vm.h>
#include <video.h>

extern uint64_t p4_table[];
extern uint64_t p3_table_hh[];
extern uint64_t p2_table[];
#if SMALL_PAGES == 1
extern uint64_t pt_tables[];
#endif

/*struct framebuffer_info framebuffer_data;*/
void map_framebuffer_2(framebuffer_info fbdata) {
}

/*void map_framebuffer_2(struct framebuffer_info fbdata) {
    uint32_t fb_entries = fbdata.memory_size / PAGE_SIZE_IN_BYTES;
    pretty_logf(Verbose, "Fbdata size: 0x%x", fbdata.memory_size);
}*/
/*
    uint32_t fb_entries = fbdata.memory_size / PAGE_SIZE_IN_BYTES;
    pretty_logf(Verbose, "Fbdata size: 0x%x", fbdata.memory_size);

    uint64_t phys_address = (uint64_t) fbdata.phys_address;

    uint32_t pd = PD_ENTRY(_FRAMEBUFFER_MEM_START);
    uint32_t pdpr = PDPR_ENTRY(_FRAMEBUFFER_MEM_START);
    uint32_t pml4 = PML4_ENTRY(_FRAMEBUFFER_MEM_START);
#if SMALL_PAGES == 1
    uint32_t fb_pd_entries = fb_entries / VM_PAGES_PER_TABLE;
    //uint32_t pt = PT_ENTRY(_FRAMEBUFFER_MEM_START);
#endif

    if(p4_table[pml4] == 0x00l || p3_table_hh[pdpr] == 0x00l){
        pretty_log(Verbose, "PANIC - PML4 or PDPR Empty - not supported for now\n");
        asm("hlt");
    }

#if SMALL_PAGES == 1
    uint64_t *current_page_table = pt_tables;
    for(uint32_t i = 0; i <= fb_pd_entries; i++){
        bool newly_allocated = false;
        // Probably should be safer to rely on the direct map if possible?
        if(p2_table[pd] == 0x00){
            uint64_t *new_table = pmm_prepare_new_pagetable();
            p2_table[pd] = (uint64_t)new_table | (PRESENT_BIT | WRITE_BIT);
            uint64_t *new_table_hhdm = hhdm_get_variable((uintptr_t)new_table);
            current_page_table = new_table_hhdm;
            clean_new_table((uint64_t *)new_table_hhdm);
            newly_allocated = true;
        }
        for(int j=0; j < VM_PAGES_PER_TABLE && fb_entries > 0; j++){
            if(newly_allocated == false){
            } else {
                current_page_table[j] = phys_address + (((VM_PAGES_PER_TABLE * i) + j) * PAGE_SIZE_IN_BYTES) | PAGE_ENTRY_FLAGS;
            }
            fb_entries--;
        }
        newly_allocated = false;
        pd++;
    }
#elif SMALL_PAGES == 0
    uint32_t fb_entries_mod =  fbdata.memory_size % PAGE_SIZE_IN_BYTES;
    if(fb_entries_mod != 0){
        fb_entries++;
    }
    for(int j=0; fb_entries > 0; j++){
        fb_entries--;
        if( (p2_table[pd+j] < phys_address
                || p2_table[pd+j] > (phys_address + fbdata.memory_size) )
                || p2_table[pd+j] == 0x00l ) {
                p2_table[pd+j] = (phys_address + (j * PAGE_SIZE_IN_BYTES)) | PAGE_ENTRY_FLAGS;
        }
    }
#endif
}

void set_fb_data(struct multiboot_tag_framebuffer *fbtag){
    //FRAMEBUFFER_MEM = (void*)(uint64_t)fbtag->common.framebuffer_addr;
#if USE_FRAMEBUFFER == 1
    framebuffer_data.address = (void*)(uint64_t)_FRAMEBUFFER_MEM_START;
    //framebuffer_data.address = hhdm_get_variable((uintptr_t) (fbtag->common.framebuffer_addr));
    framebuffer_data.pitch = fbtag->common.framebuffer_pitch;
    framebuffer_data.bpp = fbtag->common.framebuffer_bpp;
    framebuffer_data.memory_size = fbtag->common.framebuffer_pitch * fbtag->common.framebuffer_height;
    framebuffer_data.width = fbtag->common.framebuffer_width;
    framebuffer_data.height = fbtag->common.framebuffer_height;
    framebuffer_data.phys_address = fbtag->common.framebuffer_addr;

    number_of_lines = 0;

    map_framebuffer(framebuffer_data);
    cur_fb_line = 0;
    framebuffer_main_window.x_orig = 0;
    framebuffer_main_window.y_orig = 0;
    framebuffer_main_window.width = framebuffer_data.width;
    framebuffer_main_window.height = framebuffer_data.height;
    logo_area_ptr = NULL;

#endif
}
*/
