#include <kernel/framebuffer.h>
#include <kernel/video.h>

extern char _binary_fonts_default_psf_size;
extern char _binary_fonts_default_psf_start;
extern char _binary_fonts_default_psf_end;
extern uint32_t FRAMEBUFFER_PITCH;
extern void *FRAMEBUFFER_MEM;

void _fb_putchar(unsigned short int symbol, int cx, int cy, uint32_t fg, uint32_t bg){
    _printStr("Inside fb_putchar");
    char *framebuffer = (char *) FRAMEBUFFER_MEM;
    PSF_font *default_font = (PSF_font*)&_binary_fonts_default_psf_start;
    uint32_t pitch = FRAMEBUFFER_PITCH;
    uint32_t charsize = default_font->height * ((default_font->width + 7)/8);
    unsigned char *glyph = (unsigned char *)&_binary_fonts_default_psf_start + 
        default_font->headersize + (symbol>0&&symbol<default_font->numglyph?symbol:0) * default_font->bytesperglyph;
    int bytesperline =  (default_font->width + 7)/8;
    int offset = (cy * default_font->height * pitch) + 
        (cx * (default_font->width+1) * 4);
    
    int x, y, line, mask;

    for(y=0; y<default_font->height; y++){
        line = offset;
        mask = 1 << (default_font->width - 1);
        for(x=0; x<default_font->width; x++){
            *((uint32_t*) (framebuffer + line)) = ((int) *glyph) & mask ? fg : bg;
            mask >>= 1;
            line +=4;
        }
        glyph += bytesperline;
        offset +=pitch;
    }
    //TODO Compute offset
    //TODO Double cycle to print the glyph
    //  TODO: in the cycle update the mask used to display the pixel (cur_glyph_line) & mask
}

