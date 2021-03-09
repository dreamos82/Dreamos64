#ifndef _VIDEO_H
#define _VIDEO_H

#include <stdint.h> 

#define _SCR_W 80
#define _SCR_H 25

typedef enum {
    BLACK,
    BLUE,
    GREEN,
    CYAN,
    RED,
    MAGENTA,
    BROWN,
    GREY,
    DARK_GREY,
    BRIGHT_BLUE,
    BRIGHT_GREEN,
    BRIGHT_CYAN,
    BRIGHT_RED,
    BRIGHT_MAGENTA,
    YELLOW,
    WHITE
} character_color;


typedef struct {
    uint32_t magic;         /* magic bytes to identify PSF */
    uint32_t version;       /* zero */
    uint32_t headersize;    /* offset of bitmaps in file, 32 */
    uint32_t flags;         /* 0 if there's no unicode table */
    uint32_t numglyph;      /* number of glyphs */
    uint32_t bytesperglyph; /* size of each glyph */
    uint32_t height;        /* height in pixels */
    uint32_t width;         /* width in pixels */
} PSF_font;

void _printCh(char, character_color);
void _printStr(char *);
void _printNewLine();
void _printNumber(char*, unsigned int, int);
void _printHex(char*, unsigned int);
void _clearScreen();

void set_fb_data(struct multiboot_tag_framebuffer *);
char* _getHexString(char*, unsigned int);
#endif
