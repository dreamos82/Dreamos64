#ifndef _PSF_H
#define _PSF_H

#include <stdint.h>

#define MAGIC_V1_0 0x36
#define MAGIC_V1_1 0x04

#define MAGIC_V2 0x864ab572 

#define PSF1_MODE512    0x01
#define PSF1_MODEHASTAB 0x02
#define PSF1_MODEHASSEQ 0x04
#define PSF1_MAXMODE    0x05

#define PSF_V1 0x01
#define PSF_V2 0x02

typedef struct {
    uint8_t magic[2];
    uint8_t mode;
    uint8_t charsize; /* In psf v1 the width is always 8 so the height is equal the charsize*/
} PSFv1_Font;

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

uint8_t get_PSF_version(char *);

uint8_t* get_glyph(uint8_t, uint8_t);
#endif
