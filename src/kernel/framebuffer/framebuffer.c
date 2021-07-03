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

uint32_t FRAMEBUFFER_PITCH;
void *FRAMEBUFFER_MEM;
uint8_t FRAMEBUFFER_BPP = 0;
uint32_t FRAMEBUFFER_MEMORY_SIZE = 0;
uint32_t FRAMEBUFFER_WIDTH;
uint32_t FRAMEBUFFER_HEIGHT;


void map_framebuffer(struct multiboot_tag_framebuffer *tagfb){
    //TODO: implement function
    uint32_t fb_entries = FRAMEBUFFER_MEMORY_SIZE / PAGE_SIZE_IN_BYTES;
    uint32_t fb_entries_mod = FRAMEBUFFER_MEMORY_SIZE % PAGE_SIZE_IN_BYTES;
    uint32_t fb_pd_entries = fb_entries / 512;
    
    _printStringAndNumber("FB: Entries: ", fb_entries);
    _printStringAndNumber("FB: Mod Entries: ", fb_entries_mod);
    _printStringAndNumber("FB PD: Entries: ", fb_pd_entries);
    _printStringAndNumber("FB: size: ", FRAMEBUFFER_MEMORY_SIZE);
    uint64_t current_address = (void *) (uint64_t) tagfb->common.framebuffer_addr;

    uint32_t pd = PD_ENTRY(_FRAMEBUFFER_MEM_START); 
    uint32_t pdpr = PDPR_ENTRY(_FRAMEBUFFER_MEM_START);
    uint32_t pml4 = PML4_ENTRY(_FRAMEBUFFER_MEM_START);
    _printStringAndNumber("pd: ", pd);
    _printStringAndNumber("pdpr: ", pdpr);
    _printStringAndNumber("pml4: ", pml4);
    
    uint32_t entries_left = fb_entries_mod;
    uint32_t counter = 0;
    for(int i = 0; i <= fb_pd_entries; i++){
        /*if(i == fb_pd_entries){
            fb_entries = fb_entries - (i * 512);
            _printStringAndNumber("fb_entries last cycle: ", fb_entries);
        }*/

        for(int j=0; j <= 512 && fb_entries > 0; j++){
            counter++;
            fb_entries--;
        }

    }
    _printStringAndNumber("Counter value: ", counter);
    
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
    PSF_font *default_font = (PSF_font*)&_binary_fonts_default_psf_start;
    uint32_t pitch = FRAMEBUFFER_PITCH;
    //uint32_t charsize = default_font->height * ((default_font->width + 7)/8);
    uint8_t *glyph = (uint8_t*)&_binary_fonts_default_psf_start + 
        default_font->headersize + (symbol>0&&symbol<default_font->numglyph?symbol:0) * default_font->bytesperglyph;
    int bytesperline =  (default_font->width + 7)/8;
    int offset = (cy * default_font->height * pitch) + 
        (cx * (default_font->width) * sizeof(PIXEL));
    
    uint32_t x, y, line, mask;

    for(y=0; y<default_font->height; y++){
        line = offset;
        mask = 1 << (default_font->width - 1);
        for(x=0; x<default_font->width; x++){
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
    #ifdef DEBUG
		qemu_write_string(string);
        qemu_write_string("\n");
    #endif
    while (*string != '\0'){
        if (*string == '\n'){
            qemu_write_string("_fb_printStr: newline support not added yet");
        } else {
            _fb_putchar(*string, cx, cy, fg, bg);
        }
        string++;
        cx++;
    }
}

