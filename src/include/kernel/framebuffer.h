#ifndef _FRAMEBUFFER_H
#define _FRAMEBUFFER_H

#include <stdint.h>
#include <stddef.h>
#include <multiboot.h>
#include <window.h>

#define PIXEL uint32_t

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

extern _fb_window_t framebuffer_main_window;
extern _fb_window_t framebuffer_logo_area;

extern size_t cur_fb_line;
extern size_t cur_fb_column;
extern framebuffer_info framebuffer_data;
extern uint32_t number_of_lines;
extern _fb_window_t *logo_area_ptr;

void _fb_putchar(char symbol, uint32_t fg, uint32_t bg);
void _fb_putcharAt(char symbol, size_t cx, size_t cy, uint32_t fg, uint32_t bg);
void _fb_printStrAt(const char *string, size_t cx, size_t cy, uint32_t fg, uint32_t bg);
void _fb_printStr(const char *string, uint32_t fg, uint32_t bg);

void _fb_put_pixel(uint32_t, uint32_t, uint32_t);

uint32_t _fb_get_pixel(uint32_t x, uint32_t y);

void* map_framebuffer(struct framebuffer_info fbdata);
void set_fb_data(struct multiboot_tag_framebuffer *);
void _fb_printStrAndNumber(const char*, uint64_t, uint32_t, uint32_t);
void _fb_printStrAndNumberAt(const char*, uint64_t, size_t, size_t, uint32_t, uint32_t);

void get_framebuffer_mode(uint32_t* pixels_w, uint32_t* pixels_h, uint32_t* chars_w, uint32_t* chars_h);
void draw_logo(uint32_t start_x, uint32_t start_y);

void _fb_scrollLine(_fb_window_t *scrolling_window, uint32_t line_height, uint32_t number_of_lines_to_scroll, _fb_window_t *area_to_pin);
void _fb_scroll(_fb_window_t *scrolling_window, uint32_t line_height, uint32_t number_of_lines_to_scroll, _fb_window_t *area_to_pin, bool clear_last_line);

#endif
