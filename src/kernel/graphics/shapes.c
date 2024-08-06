#include <framebuffer.h>
#include <shapes.h>

void _fb_draw_rectangle(_fb_window_t main_window, _fb_window_t rectangle, uint32_t color) {
    //0xEB72F9
    uint32_t rectangle_w = rectangle.x_orig + rectangle.width;
    uint32_t rectangle_h = rectangle.y_orig + rectangle.height;
    for(uint32_t i = rectangle.y_orig; i < rectangle_h; i++) {
        for(uint32_t j = rectangle.x_orig; j < rectangle_w; j++) {
            _fb_put_pixel(j, i, color);
        }
    }
}
