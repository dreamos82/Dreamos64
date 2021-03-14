#ifndef _FRAMEBUFFER_H
#define _FRAMEBUFFER_H

#include <stdint.h>

#define PIXEL uint32_t

void _fb_putchar(unsigned short int symbol, int cx, int cy, uint32_t fg, uint32_t bg);
void _fb_printStr(char *string, int cx, int cy, uint32_t fg, uint32_t bg);

#endif
