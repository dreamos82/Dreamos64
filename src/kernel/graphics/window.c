#include <logging.h>
#include <window.h>


/**
 * This function return true if x and y are within the area of the window defined by area_to_intersect
 *
 *
 * @param x x coord of the point to check
 * @param pixels_h y cood of the point to check
 * @param area_to_intersect the window area we want to check the intersection
 * @return true if the point (x,y) is within the rectangle specified by area_to_intersect
 */
bool _fb_intersect_window(uint32_t x, uint32_t y, _fb_window_t *area_to_intersect) {
    if ( area_to_intersect == NULL ) {
        return false;
    }
    if ( x > area_to_intersect->x_orig + area_to_intersect->width)
        return false;
    if ( x < area_to_intersect->x_orig)
        return false;
    if (y > area_to_intersect->y_orig + area_to_intersect->height)
        return false;
    if (y < area_to_intersect->y_orig)
        return false;
    return true;
}

/**
 * This function return true if x and y are within the area of the window defined by area_to_intersect
 *
 *
 * @param x x coord of the point to check
 * @param pixels_h y cood of the point to check
 * @param area_to_intersect the window area we want to check the intersection
 * @return true if the point (x,y) is within the rectangle specified by area_to_intersect
 */
uint8_t _fb_get_rectangles(_fb_window_t *rectangles, _fb_window_t* main_window, _fb_window_t *area_to_intersect) {
    //_fb_window_t rectangles[4];
    uint8_t cur_rectangle = 0;
    if(area_to_intersect == NULL) {
        return 0;
    }
    // We can have up to 4 rectangles surrounding an area.
    // How many depends on where the area.
    // The window and the area have the left side with same origin
    pretty_logf(Verbose, "Main window: x %d, y %d, w %d, h %d", main_window->x_orig, main_window->y_orig, main_window->width, main_window->height);
    pretty_logf(Verbose, "Area to intersect: x %d, y %d, w %d, h %d", area_to_intersect->x_orig, area_to_intersect->y_orig, area_to_intersect->width, area_to_intersect->height);
    if (main_window->x_orig < area_to_intersect->x_orig) {
        rectangles[cur_rectangle].x_orig = main_window->x_orig;
        rectangles[cur_rectangle].y_orig = main_window->y_orig;
        rectangles[cur_rectangle].width = area_to_intersect->x_orig - main_window->x_orig;
        rectangles[cur_rectangle].height = main_window->height;
        pretty_logf(Verbose, "1 rectangle: %d, x_orig: %d, y_orig: %d, widht: %d, height: %d", cur_rectangle, rectangles[cur_rectangle].x_orig, rectangles[cur_rectangle].y_orig, rectangles[cur_rectangle].width, rectangles[cur_rectangle].height);
        cur_rectangle++;
    }
    if (area_to_intersect->x_orig + area_to_intersect->width < main_window->x_orig + main_window->width) {
        rectangles[cur_rectangle].x_orig = area_to_intersect->x_orig + area_to_intersect->width;
        rectangles[cur_rectangle].y_orig = main_window->y_orig;
        rectangles[cur_rectangle].width = (main_window->x_orig + main_window->width) - (area_to_intersect->x_orig + area_to_intersect->width);
        rectangles[cur_rectangle].height = main_window->height;
        pretty_logf(Verbose, "2 rectangle: %d, x_orig: %d, y_orig: %d, widht: %d, height: %d", cur_rectangle, rectangles[cur_rectangle].x_orig, rectangles[cur_rectangle].y_orig, rectangles[cur_rectangle].width, rectangles[cur_rectangle].height);
        cur_rectangle++;
    }
    if ( main_window->y_orig  < area_to_intersect->y_orig) {
        rectangles[cur_rectangle].x_orig = area_to_intersect->x_orig;
        rectangles[cur_rectangle].width = area_to_intersect->width;
        rectangles[cur_rectangle].y_orig = main_window->y_orig;
        rectangles[cur_rectangle].height = area_to_intersect->y_orig - main_window->y_orig;
        pretty_logf(Verbose, "3 rectangle: %d, x_orig: %d, y_orig: %d, widht: %d, height: %d", cur_rectangle, rectangles[cur_rectangle].x_orig, rectangles[cur_rectangle].y_orig, rectangles[cur_rectangle].width, rectangles[cur_rectangle].height);
        cur_rectangle++;

    }
    if ( (area_to_intersect->y_orig + area_to_intersect->height) < (main_window->y_orig + main_window->height) ) {
        rectangles[cur_rectangle].x_orig = area_to_intersect->x_orig;
        rectangles[cur_rectangle].width = area_to_intersect->width;
        rectangles[cur_rectangle].y_orig = (area_to_intersect->y_orig + area_to_intersect->height);
        rectangles[cur_rectangle].height = (main_window->y_orig + main_window->height) - (area_to_intersect->y_orig + area_to_intersect->height);
        pretty_logf(Verbose, "4 rectangle: %d, x_orig: %d, y_orig: %d, widht: %d, height: %d", cur_rectangle, rectangles[cur_rectangle].x_orig, rectangles[cur_rectangle].y_orig, rectangles[cur_rectangle].width, rectangles[cur_rectangle].height);
        cur_rectangle++;
    }
    return cur_rectangle;
}
