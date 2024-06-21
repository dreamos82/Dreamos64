#ifndef __WINDOW_H__
#define __WINDOW_H__

#include <stdbool.h>
#include <stdint.h>

/**
 * This struct defines a rectangular area within the framebuffer
 */
typedef struct _fb_window_t {
    uint32_t x_orig; /**< The x coordinate of the top-left point of the window */
    uint32_t y_orig; /**< The y coordinate of the top-left point of the window */

    uint32_t width; /**< The width of the window */
    uint32_t height; /**< The height of the window */
    //void *window_address;
} _fb_window_t;


bool _fb_intersect_window(uint32_t x, uint32_t y, _fb_window_t *area_to_interesect);

#endif
