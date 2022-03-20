#include <multiboot.h>
#include <kernel/framebuffer.h>
#include <kernel/video.h>
#include <kernel/psf.h>
#include <kernel/mem/bitmap.h>
#include <vm.h>
//#ifdef DEBUG - This will be uncommented when the framebuffer library will be completed
#include <qemu.h>
//#endif

extern char _binary_fonts_default_psf_size;
extern char _binary_fonts_default_psf_start;
extern char _binary_fonts_default_psf_end;
extern void *cur_framebuffer_pos;
extern uint64_t p4_table[];
extern uint64_t p3_table_hh[];
extern uint64_t p2_table[];
extern uint64_t pt_tables[];
extern uint8_t psf_font_version;

uint32_t FRAMEBUFFER_PITCH;
void *FRAMEBUFFER_MEM;
uint8_t FRAMEBUFFER_BPP = 0;
uint32_t FRAMEBUFFER_MEMORY_SIZE = 0;
uint32_t FRAMEBUFFER_WIDTH;
uint32_t FRAMEBUFFER_HEIGHT;


void map_framebuffer(struct multiboot_tag_framebuffer *tagfb){
    uint32_t fb_entries = FRAMEBUFFER_MEMORY_SIZE / PAGE_SIZE_IN_BYTES;
    uint32_t fb_entries_mod = FRAMEBUFFER_MEMORY_SIZE % PAGE_SIZE_IN_BYTES;
    
    uint64_t phys_address = (uint64_t) tagfb->common.framebuffer_addr;

    uint32_t pd = PD_ENTRY(_FRAMEBUFFER_MEM_START); 
    uint32_t pdpr = PDPR_ENTRY(_FRAMEBUFFER_MEM_START);
    uint32_t pml4 = PML4_ENTRY(_FRAMEBUFFER_MEM_START);

#if SMALL_PAGES == 1
    uint32_t fb_pd_entries = fb_entries / VM_PAGES_PER_TABLE;
    uint32_t pt = PT_ENTRY(_FRAMEBUFFER_MEM_START);
#endif
    
    uint32_t counter = 0;

    if(p4_table[pml4] == 0x00l || p3_table_hh[pdpr] == 0x00l){
        _printStr("PANIC - PML4 or PDPR Empty - not supported for now\n");
        asm("hlt");
    }

#if SMALL_PAGES == 1
    uint64_t *current_page_table = pt_tables;
    for(uint32_t i = 0; i <= fb_pd_entries; i++){
        bool newly_allocated = false;
        if(p2_table[pd] == 0x00){
            uint64_t *new_table = pmm_alloc_frame();
            p2_table[pd] = (uint64_t)new_table | (PRESENT_BIT | WRITE_BIT);
            current_page_table = new_table;
            clean_new_table((uint64_t *)new_table);
            newly_allocated = true;
        }
        for(int j=0; j < VM_PAGES_PER_TABLE && fb_entries > 0; j++){
            if(newly_allocated == false){                
                counter++;
            } else {                
                current_page_table[j] = phys_address + (((VM_PAGES_PER_TABLE * i) + j) * PAGE_SIZE_IN_BYTES) | PAGE_ENTRY_FLAGS;
            }
            fb_entries--;            
        }
        newly_allocated = false;
        pd++;

    }
#elif SMALL_PAGES == 0
    if(fb_entries_mod != 0){
        fb_entries++;
    }
    for(int j=0; fb_entries > 0; j++){
        counter++;
        fb_entries--;
        if((p2_table[pd+j] < phys_address 
                    || p2_table[pd+j] > (phys_address + FRAMEBUFFER_MEMORY_SIZE)) 
                || p2_table[pd+j] == 0x00l){
            p2_table[pd+j] = (phys_address + (j * PAGE_SIZE_IN_BYTES)) | PAGE_ENTRY_FLAGS;
        }
    }


#endif
}


void set_fb_data(struct multiboot_tag_framebuffer *fbtag){
    //FRAMEBUFFER_MEM = (void*)(uint64_t)fbtag->common.framebuffer_addr;
    FRAMEBUFFER_MEM = (void*)(uint64_t)_FRAMEBUFFER_MEM_START;
    FRAMEBUFFER_PITCH = fbtag->common.framebuffer_pitch;
    FRAMEBUFFER_BPP = fbtag->common.framebuffer_bpp;
    FRAMEBUFFER_MEMORY_SIZE = FRAMEBUFFER_PITCH * fbtag->common.framebuffer_height;
    FRAMEBUFFER_WIDTH = fbtag->common.framebuffer_width;
    FRAMEBUFFER_HEIGHT = fbtag->common.framebuffer_height;
}

void _fb_putchar(unsigned short int symbol, int cx, int cy, uint32_t fg, uint32_t bg){
    char *framebuffer = (char *) FRAMEBUFFER_MEM;
    uint32_t pitch = FRAMEBUFFER_PITCH;
    uint32_t width, height;
    width = get_width(psf_font_version);
    height = get_height(psf_font_version);

    //uint32_t charsize = default_font->height * ((default_font->width + 7)/8);
    //uint8_t *glyph = (uint8_t*)&_binary_fonts_default_psf_start + 
    //    default_font->headersize + (symbol>0&&symbol<default_font->numglyph?symbol:0) * default_font->bytesperglyph;
    uint8_t *glyph = (uint8_t*) get_glyph(symbol, psf_font_version);
    int bytesperline =  (width + 7)/8;
    int offset = (cy * height * pitch) + 
        (cx * (width) * sizeof(PIXEL));
    
    uint32_t x, y, line, mask;
    for(y=0; y<height; y++){
        line = offset;
        mask = 1 << (width - 1);
        for(x=0; x<width; x++){
            //*((uint32_t*) (framebuffer + line)) = *((unsigned int*) glyph) & mask ? fg : bg;
            *((PIXEL*) (framebuffer + line)) = glyph[x/8] & (0x80 >> (x & 7)) ? fg : bg;
            mask >>= 1;
            line +=sizeof(PIXEL);
        }
        glyph += bytesperline;
        offset +=pitch;
    }
}

void _fb_printStr(char *string, int cx, int cy, uint32_t fg, uint32_t bg){
    while (*string != '\0'){
        if (*string == '\n'){
            qemu_write_string("_fb_printStr: newline support not added yet\n");
            cx=0;
            cy++;
        } else {
            _fb_putchar(*string, cx, cy, fg, bg);
            cx++;
        }
        string++;
    }
}

void _fb_printStrAndNumber(char *string, uint64_t number, int cx, int cy, uint32_t fg, uint32_t bg){
    char *buffer[30];
    
    _getHexString(buffer, number, true);
    _fb_printStr(string, cx, cy, fg, bg);
    int counter = 0;
    while(*string != '\0') {
        counter++;
        string++;
    }
    string[counter] = ' ';
    _fb_printStr(buffer, cx + counter, cy, fg, bg);
}

void get_framebuffer_mode(uint32_t* pixels_w, uint32_t* pixels_h, uint32_t* chars_w, uint32_t* chars_h)
{
    if (pixels_w != NULL)
        *pixels_w = FRAMEBUFFER_WIDTH;
    if (pixels_h != NULL)
        *pixels_h = FRAMEBUFFER_HEIGHT;

    if (chars_w != NULL)
        *chars_w = FRAMEBUFFER_WIDTH / get_width(psf_font_version);
    if (chars_h != NULL)
        *chars_h = FRAMEBUFFER_HEIGHT / get_height(get_height);
}
