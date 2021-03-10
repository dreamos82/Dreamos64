#include <kernel/framebuffer.h>
#include <kernel/video.h>

extern char _binary_fonts_default_psf_size;
extern char _binary_fonts_default_psf_start;
extern char _binary_fonts_default_psf_end;
extern uint32_t FRAMEBUFFER_PITCH;

void _fb_putchar(unsigned short int symbol){

    PSF_font *default_font = (PSF_font*)&_binary_fonts_default_psf_start;

    unsigned char *glyph = (unsigned char)&_binary_fonts_default_psf_start + 
        default_font->headersize + symbol + default_font->bytesperglyph;
}

