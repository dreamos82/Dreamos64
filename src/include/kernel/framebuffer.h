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

    uint32_t number_of_lines;
    uint32_t number_of_rows;

    uint64_t phys_address;
} framebuffer_info;

typedef struct _fb_window_t {
    uint32_t x_orig;
    uint32_t y_orig;

    uint32_t width;
    uint32_t height;

    void *window_address;

} _fb_window_t;

void _fb_putchar(char symbol, size_t cx, size_t cy, uint32_t fg, uint32_t bg);
void _fb_printStrAt(const char *string, size_t cx, size_t cy, uint32_t fg, uint32_t bg);
void _fb_printStr(const char *string, uint32_t fg, uint32_t bg);
void _fb_put_pixel(uint32_t, uint32_t, uint32_t);

/*void map_framebuffer(struct multiboot_tag_framebuffer *);*/
void set_fb_data(struct multiboot_tag_framebuffer *);
void _fb_printStrAndNumber(const char*, uint64_t, uint32_t, uint32_t);
void _fb_printStrAndNumberAt(const char*, uint64_t, size_t, size_t, uint32_t, uint32_t);

void get_framebuffer_mode(uint32_t* pixels_w, uint32_t* pixels_h, uint32_t* chars_w, uint32_t* chars_h);
void draw_logo(uint32_t start_x, uint32_t start_y);

void _fb_scrollLine(uint32_t x_origin, uint32_t y_origin, uint32_t window_width, uint32_t window_height, uint32_t line_height, uint32_t number_of_lines_to_scroll);
#endif
