#include <assert.h>
#include <test_common.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <window.h>

void test_get_rectangles();

int main() {
    test_get_rectangles();
}

void test_get_rectangles() {
    _fb_window_t rectangles[4];
    _fb_window_t main_window = {0,0, 1280, 800};
    _fb_window_t area_to_intersect = {1030, 0, 250, 250};
    printf("Testing _fb_get_rectangles function\n");
    printf("\t[%s]: main_window values: x: %d - y: %d - w: %d - h: %d\n", __FUNCTION__, main_window.x_orig, main_window.y_orig, main_window.width, main_window.height);
    uint8_t n_squares = _fb_get_rectangles(rectangles, &main_window, NULL);
    pretty_assert(0, n_squares, ==, "Testing number of rectangles returned. Passing NULL");
    n_squares = _fb_get_rectangles(rectangles, &main_window, &area_to_intersect);
    pretty_assert(2, n_squares, ==, "Testing number of rectangles returned.");
    pretty_assert( 0, rectangles[0].x_orig, ==, "Testing rectangles[0].x_orig");
    pretty_assert( 0, rectangles[0].y_orig, ==, "Testing rectangles[0].y_orig");
    pretty_assert( 1030, rectangles[0].width, ==, "Testing rectangles[0].width");
    pretty_assert( 800, rectangles[0].height, ==, "Testing rectangles[0].height");
    pretty_assert( 1030, rectangles[1].x_orig, ==, "Testing rectangles[1].x_orig");
    pretty_assert( 250, rectangles[1].y_orig, ==, "Testing rectangles[1].y_orig");
    pretty_assert( 250, rectangles[1].width, ==, "Testing rectangles[1].width");
    pretty_assert( 550, rectangles[1].height, ==, "Testing rectangles[1].height");
    _fb_window_t area_to_intersect_2 = {500, 300, 200, 120};
    n_squares = _fb_get_rectangles(rectangles, &main_window, &area_to_intersect_2);
    printf("\t[%s]: area_to_intersect values: x: %d - y: %d - w: %d - h: %d\n", __FUNCTION__, area_to_intersect_2.x_orig, area_to_intersect_2.y_orig, area_to_intersect_2.width,
    area_to_intersect_2.height);
    pretty_assert(4, n_squares, ==, "Testing number of rectangles returned.");
    _fb_window_t four_results[4] = { {0, 0, 500, 800}, {700, 0, 580, 800 }, {500, 0, 200, 300}, {500, 420, 200, 380} };
    for(int i = 0; i < n_squares; i++) {
        printf("\t[%s]: item: %d) x_orig: %d - y_orig: %d - width: %d - height: %d\n", __FUNCTION__, i, rectangles[i].x_orig, rectangles[i].y_orig, rectangles[i].width, rectangles[i].height);
        pretty_assert( four_results[i].x_orig, rectangles[i].x_orig, ==, "Testing rectangles[i].x_orig");
        pretty_assert( four_results[i].y_orig, rectangles[i].y_orig, ==, "Testing rectangles[i].y_orig");
        pretty_assert( four_results[i].width, rectangles[i].width, ==, "Testing rectangles[i].width");
        pretty_assert( four_results[i].height, rectangles[i].height, ==, "Testing rectangles[i].height");
    }
    _fb_window_t area_to_intersect_3 = {0, 0, 200, 120};
    n_squares = _fb_get_rectangles(rectangles, &main_window, &area_to_intersect_3);
    pretty_assert(2, n_squares, ==, "Testing number of rectangles returned.");
    _fb_window_t four_results_second[2] = { {200, 0, 1080, 800}, {0, 120, 200, 680 }};
    for(int i = 0; i < n_squares; i++) {
        printf("\t[%s]: item: %d) x_orig: %d - y_orig: %d - width: %d - height: %d\n", __FUNCTION__, i, rectangles[i].x_orig, rectangles[i].y_orig, rectangles[i].width, rectangles[i].height);
        pretty_assert( four_results_second[i].x_orig, rectangles[i].x_orig, ==, "Testing rectangles[i].x_orig");
        pretty_assert( four_results_second[i].y_orig, rectangles[i].y_orig, ==, "Testing rectangles[i].y_orig");
        pretty_assert( four_results_second[i].width, rectangles[i].width, ==, "Testing rectangles[i].width");
        pretty_assert( four_results_second[i].height, rectangles[i].height, ==, "Testing rectangles[i].height");
    }
}
