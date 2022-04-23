#ifndef _FRAMEBUFFER_H
#define _FRAMEBUFFER_H

#include <stdint.h>
#include <stddef.h>
#include <multiboot.h>


#define _FRAMEBUFFER_MEM_START 0xffffffffbd000000

typedef struct framebuffer_info {
    void *address;
    uint8_t bpp;
    uint32_t pitch;
    uint32_t memory_size;
    uint32_t width;
    uint32_t height;

    uint64_t phys_address;
} framebuffer_info;

void _fb_putchar(char symbol, size_t cx, size_t cy, uint32_t fg, uint32_t bg);
void _fb_printStr(const char *string, size_t cx, size_t cy, uint32_t fg, uint32_t bg);
void _fb_put_pixel(uint32_t, uint32_t, uint32_t);

/*void map_framebuffer(struct multiboot_tag_framebuffer *);*/
void set_fb_data(struct multiboot_tag_framebuffer *);
void _fb_printStrAndNumber(const char*, uint64_t, size_t, size_t, uint32_t, uint32_t);

void get_framebuffer_mode(uint32_t* pixels_w, uint32_t* pixels_h, uint32_t* chars_w, uint32_t* chars_h);
void draw_logo();
#endif
