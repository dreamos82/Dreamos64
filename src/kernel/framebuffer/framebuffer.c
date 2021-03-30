#include <multiboot.h>
#include <kernel/framebuffer.h>
#include <kernel/video.h>
#include <kernel/psf.h>

extern char _binary_fonts_default_psf_size;
extern char _binary_fonts_default_psf_start;
extern char _binary_fonts_default_psf_end;
extern void *cur_framebuffer_pos;

uint32_t FRAMEBUFFER_PITCH;
void *FRAMEBUFFER_MEM;
uint8_t FRAMEBUFFER_BPP = 0;

void set_fb_data(struct multiboot_tag_framebuffer *fbtag){
    //FRAMEBUFFER_MEM = (void*)(uint64_t)fbtag->common.framebuffer_addr;
    FRAMEBUFFER_MEM = (void*)(uint64_t)_FRAMEBUFFER_MEM_START;
    FRAMEBUFFER_PITCH = fbtag->common.framebuffer_pitch;
    FRAMEBUFFER_BPP = fbtag->common.framebuffer_bpp;
}

void _fb_putchar(unsigned short int symbol, int cx, int cy, uint32_t fg, uint32_t bg){
    char *framebuffer = (char *) FRAMEBUFFER_MEM;
    PSF_font *default_font = (PSF_font*)&_binary_fonts_default_psf_start;
    uint32_t pitch = FRAMEBUFFER_PITCH;
    uint32_t charsize = default_font->height * ((default_font->width + 7)/8);
    uint8_t *glyph = (uint8_t*)&_binary_fonts_default_psf_start + 
        default_font->headersize + (symbol>0&&symbol<default_font->numglyph?symbol:0) * default_font->bytesperglyph;
    int bytesperline =  (default_font->width + 7)/8;
    int offset = (cy * default_font->height * pitch) + 
        (cx * (default_font->width) * sizeof(PIXEL));
    
    int x, y, line, mask;

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

