#include <window.h>


/**
 * This function return true if x and y are within the area of the window defined by area_to_interesect
 *
 *
 * @param x x coord of the point to check
 * @param pixels_h y cood of the point to check
 * @param area_to_interesect the window area we want to check the intersection
 * @return true if the point (x,y) is within the rectangle specified by area_to_interesect
 */
bool _fb_intersect_window(uint32_t x, uint32_t y, _fb_window_t *area_to_interesect) {
    if ( x > area_to_interesect->x_orig + area_to_interesect->width)
        return false;
    if ( x < area_to_interesect->x_orig)
        return false;
    if (y > area_to_interesect->y_orig + area_to_interesect->height)
        return false;
    if (y < area_to_interesect->y_orig)
        return false;
    return true;
}
