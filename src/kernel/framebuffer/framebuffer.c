#include <multiboot.h>
#include <framebuffer.h>
#include <hh_direct_map.h>
#include <pmm.h>
#include <psf.h>
#include <bitmap.h>
#include <vm.h>
#include <stdio.h>
#include <numbers.h>
//#ifdef DEBUG - This will be uncommented when the framebuffer library will be completed
#include <qemu.h>
//#endif
#include <video.h>
#include <dreamcatcher.h>
#include <logging.h>

#define PIXEL uint32_t

extern void *cur_framebuffer_pos;
extern uint64_t p4_table[];
extern uint64_t p3_table_hh[];
extern uint64_t p2_table[];
extern uint64_t pt_tables[];
extern uint8_t psf_font_version;

uint32_t FRAMEBUFFER_PITCH;
void *FRAMEBUFFER_MEM;
uint8_t FRAMEBUFFER_BPP = 0;
uint32_t FRAMEBUFFER_MEMORY_SIZE = 0;
uint32_t FRAMEBUFFER_WIDTH;
uint32_t FRAMEBUFFER_HEIGHT;
struct framebuffer_info framebuffer_data;

size_t cur_fb_line;
uint32_t number_of_lines;

_fb_window_t framebuffer_main_window;
_fb_window_t framebuffer_logo_area;

void map_framebuffer(struct framebuffer_info fbdata) {
    uint32_t fb_entries = fbdata.memory_size / PAGE_SIZE_IN_BYTES;

    uint64_t phys_address = (uint64_t) fbdata.phys_address;

    uint32_t pd = PD_ENTRY(_FRAMEBUFFER_MEM_START);
    uint32_t pdpr = PDPR_ENTRY(_FRAMEBUFFER_MEM_START);
    uint32_t pml4 = PML4_ENTRY(_FRAMEBUFFER_MEM_START);
#if SMALL_PAGES == 1
    uint32_t fb_pd_entries = fb_entries / VM_PAGES_PER_TABLE;
    //uint32_t pt = PT_ENTRY(_FRAMEBUFFER_MEM_START);
#endif

    if(p4_table[pml4] == 0x00l || p3_table_hh[pdpr] == 0x00l){
        pretty_log(Verbose, "PANIC - PML4 or PDPR Empty - not supported for now\n");
        asm("hlt");
    }

#if SMALL_PAGES == 1
    uint64_t *current_page_table = pt_tables;
    for(uint32_t i = 0; i <= fb_pd_entries; i++){
        bool newly_allocated = false;
        // Probably should be safer to rely on the direct map if possible?
        if(p2_table[pd] == 0x00){
            uint64_t *new_table = pmm_prepare_new_pagetable();
            p2_table[pd] = (uint64_t)new_table | (PRESENT_BIT | WRITE_BIT);
            uint64_t *new_table_hhdm = hhdm_get_variable((uintptr_t)new_table);
            current_page_table = new_table_hhdm;
            clean_new_table((uint64_t *)new_table_hhdm);
            newly_allocated = true;
        }
        for(int j=0; j < VM_PAGES_PER_TABLE && fb_entries > 0; j++){
            if(newly_allocated == false){
            } else {
                current_page_table[j] = phys_address + (((VM_PAGES_PER_TABLE * i) + j) * PAGE_SIZE_IN_BYTES) | PAGE_ENTRY_FLAGS;
            }
            fb_entries--;
        }
        newly_allocated = false;
        pd++;

    }
#elif SMALL_PAGES == 0
    uint32_t fb_entries_mod =  fbdata.memory_size % PAGE_SIZE_IN_BYTES;
    if(fb_entries_mod != 0){
        fb_entries++;
    }
    for(int j=0; fb_entries > 0; j++){
        fb_entries--;
        if( (p2_table[pd+j] < phys_address
                || p2_table[pd+j] > (phys_address + fbdata.memory_size) )
                || p2_table[pd+j] == 0x00l ) {
                p2_table[pd+j] = (phys_address + (j * PAGE_SIZE_IN_BYTES)) | PAGE_ENTRY_FLAGS;
        }
    }


#endif
}


void set_fb_data(struct multiboot_tag_framebuffer *fbtag){
    //FRAMEBUFFER_MEM = (void*)(uint64_t)fbtag->common.framebuffer_addr;
#if USE_FRAMEBUFFER == 1
    framebuffer_data.address = (void*)(uint64_t)_FRAMEBUFFER_MEM_START;
    //framebuffer_data.address = hhdm_get_variable((uintptr_t) (fbtag->common.framebuffer_addr));
    framebuffer_data.pitch = fbtag->common.framebuffer_pitch;
    framebuffer_data.bpp = fbtag->common.framebuffer_bpp;
    framebuffer_data.memory_size = fbtag->common.framebuffer_pitch * fbtag->common.framebuffer_height;
    framebuffer_data.width = fbtag->common.framebuffer_width;
    framebuffer_data.height = fbtag->common.framebuffer_height;
    framebuffer_data.phys_address = fbtag->common.framebuffer_addr;

    number_of_lines = 0;

    map_framebuffer(framebuffer_data);
    cur_fb_line = 0;
    framebuffer_main_window.x_orig = 0;
    framebuffer_main_window.y_orig = 0;
    framebuffer_main_window.width = framebuffer_data.width;
    framebuffer_main_window.height = framebuffer_data.height;

#endif
}

void _fb_putchar(char symbol, size_t cx, size_t cy, uint32_t fg, uint32_t bg){
    uint8_t *framebuffer = (uint8_t *) framebuffer_data.address;
    uint32_t pitch = framebuffer_data.pitch;
    uint32_t width, height;
    width = _psf_get_width(psf_font_version);
    height = _psf_get_height(psf_font_version);

    //uint32_t charsize = default_font->height * ((default_font->width + 7)/8);
    //uint8_t *glyph = (uint8_t*)&_binary_fonts_default_psf_start +
    //    default_font->headersize + (symbol>0&&symbol<default_font->numglyph?symbol:0) * default_font->bytesperglyph;
    uint8_t *glyph = _psf_get_glyph(symbol, psf_font_version);
    //bytesperline is the number of bytes per each row of the glyph
    size_t bytesperline =  (width + 7)/8;
    size_t offset = (cy * height * pitch) +
        (cx * (width) * sizeof(PIXEL));
    // x,y = current coordinates on the glyph bitmap

    uint32_t x, y, line;
    //uint32_t mask;
    for(y=0; y<height; y++){
        line = offset;
        //mask = 1 << (width - 1);
        for(x=0; x<width; x++){
            //*((uint32_t*) (framebuffer + line)) = *((unsigned int*) glyph) & mask ? fg : bg;
            //We are plotting the pixel
            //0x80 = 0b10000000, it is shifted right at every iteration this for loop.
            //glyph[x/8] if widht > 8, x/8 = byte selector
            //(ie width = 16bits, when x < 8, x/8, so we read glyph[0]. if x>8 then x/8 = 1, and we read glyph[1]
            //if the bit at position x is 1 plot the foreground color if is 0 plot the background color
            *((PIXEL*) (framebuffer + line)) = glyph[x/8] & (0x80 >> (x & 7)) ? fg : bg;
            //mask >>= 1;
            line +=sizeof(framebuffer_data.bpp / 8);
        }
        glyph += bytesperline;
        offset +=pitch;
    }
}

void _fb_printStr( const char *string, uint32_t fg, uint32_t bg ) {
    _fb_printStrAt(string, 0, cur_fb_line, fg, bg);
    cur_fb_line++;
    if ( cur_fb_line >= framebuffer_data.number_of_lines ) {
        pretty_log(Verbose, "Exceeding number of lines, calling scroll_function");
        _fb_scrollLine(&framebuffer_main_window, _psf_get_height(psf_font_version), 1, &framebuffer_logo_area);
        cur_fb_line--;
    }
}

void _fb_printStrAndNumber(const char *string, uint64_t number, uint32_t fg, uint32_t bg) {
    _fb_printStrAndNumberAt(string, number, 0, cur_fb_line, fg, bg);
    cur_fb_line++;
    if ( cur_fb_line >= framebuffer_data.number_of_lines ) {
        pretty_log(Verbose, "Exceeding number of lines, calling scroll_function");
        _fb_scrollLine(&framebuffer_main_window, _psf_get_height(psf_font_version), 1, &framebuffer_logo_area);
        cur_fb_line--;
    }
}

void _fb_printStrAt( const char *string, size_t cx, size_t cy, uint32_t fg, uint32_t bg ){
    while (*string != '\0'){
        if (*string == '\n'){
            cx=0;
            cy++;
            cur_fb_line = cy;
        } else if (*string == '\t'){
            cx += 4;
        } else {
            _fb_putchar(*string, cx, cy, fg, bg);
            cx++;
        }
        string++;
    }
}

void _fb_printStrAndNumberAt(const char *string, uint64_t number, size_t cx, size_t cy, uint32_t fg, uint32_t bg){
    char buffer[30];

    _getHexString(buffer, number, true);
    _fb_printStrAt(string, cx, cy, fg, bg);
    int counter = 0;
    while(*string != '\0') {
        counter++;
        string++;
    }

    _fb_printStrAt(buffer, cx + counter, cy, fg, bg);
}

/**
 * This function return the hhdm pointer of the physycal address provided
 *
 *
 * @param pixels_w this parameter will contain the width in pixels
 * @param pixels_h this parameter will contain the height in pixels
 * @param chars_w this parameter will contain the number of columns according to the char loaded
 * @param chars_h this parameter will contain the number of rows according to the char loaded
 */
void get_framebuffer_mode(uint32_t* pixels_w, uint32_t* pixels_h, uint32_t* chars_w, uint32_t* chars_h)
{
    if (pixels_w != NULL)
        *pixels_w = framebuffer_data.width;
    if (pixels_h != NULL)
        *pixels_h = framebuffer_data.height;

    if (chars_w != NULL)
        *chars_w = framebuffer_data.width / _psf_get_width(psf_font_version);
    if (chars_h != NULL)
        *chars_h = framebuffer_data.height / _psf_get_height(psf_font_version);
}

void _fb_put_pixel(uint32_t x, uint32_t y, uint32_t color) {
    uint32_t cy = y * framebuffer_data.pitch;
    uint32_t cx = x * sizeof(PIXEL);
    char *framebuffer = (char *) framebuffer_data.address;
    *((PIXEL*) (framebuffer + cy + cx)) = color;
}

void draw_logo(uint32_t start_x, uint32_t start_y) {
    pretty_logf(Verbose, "Drawing logo at x:%d y: %d", start_x, start_y );
    framebuffer_logo_area.x_orig = start_x;
    framebuffer_logo_area.y_orig = start_y;
    framebuffer_logo_area.width = width;
    framebuffer_logo_area.height = height;
    char *logo_data = header_data;
    char *data = header_data;
    unsigned char pixel[4];
    for (uint32_t i = 0; i < height; i++) {
        for(uint32_t j = 0; j < width; j++) {
            HEADER_PIXEL(logo_data, pixel);
            pixel[3] = 0;
            uint32_t num =  ((uint32_t)pixel[3] << 24) |
              ((uint32_t)pixel[0] << 16) |
              ((uint32_t)pixel[1] << 8)  |
              (uint32_t)pixel[2];
            _fb_put_pixel(start_x + j, start_y + i, num);
        }
    }
}

void _fb_scrollLine(_fb_window_t *scrolling_window, uint32_t line_height, uint32_t number_of_lines_to_scroll, _fb_window_t *area_to_pin) {
    uint32_t *framebuffer = (uint32_t *) framebuffer_data.address;
    uint32_t cur_x = scrolling_window->x_orig;
    uint32_t cur_y = scrolling_window->y_orig;
    uint32_t line_s, line_d;
    size_t offset =  (line_height*scrolling_window->width);
    line_s = scrolling_window->y_orig * framebuffer_data.pitch;
    line_d = number_of_lines_to_scroll * offset;
    uint32_t line_total_height = line_height * number_of_lines;
    bool to_ignore = false;

    while ( cur_y < (scrolling_window->height - line_total_height) ) {
        while (cur_x < scrolling_window->width) {
            to_ignore = _fb_intersect_window(cur_x, cur_y, area_to_pin);
            if (!to_ignore) {
                *((PIXEL*) framebuffer + (uint32_t)line_s) = *((PIXEL*) framebuffer + (uint32_t)line_d);
            }
            line_s++;
            line_d++;
            cur_x++;
        }
        cur_y++;
        cur_x = scrolling_window->x_orig;
    }
}
