#ifndef _FRAMEBUFFER_H
#define _FRAMEBUFFER_H

#include <stdint.h>
#include <multiboot.h>

#define PIXEL uint32_t

#define _FRAMEBUFFER_MEM_START 0xffffffffbd000000

void _fb_putchar(unsigned short int symbol, int cx, int cy, uint32_t fg, uint32_t bg);
void _fb_printStr(char *string, int cx, int cy, uint32_t fg, uint32_t bg);

void map_framebuffer(struct multiboot_tag_framebuffer *);
void set_fb_data(struct multiboot_tag_framebuffer *);
#endif
