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
    /*main_window.x_orig = 0;
    main_window.y_orig = 0;
    main_window.width = 1280;
    main_window.height = 800;*/
    _fb_window_t area_to_intersect = {1030, 0, 250, 250};
    printf("Testing _fb_get_rectangles function\n");
    printf("\t[%s]: main_window values: x: %d - y: %d - w: %d - h: %d\n", __FUNCTION__, main_window.x_orig, main_window.y_orig, main_window.width, main_window.height);
    uint8_t n_squares = _fb_get_rectangles(rectangles, &main_window, NULL);
    printf("\t[%s] n_squares should be 0 : %d\n", __FUNCTION__, n_squares);
    assert(n_squares == 0);
    n_squares = _fb_get_rectangles(rectangles, &main_window, &area_to_intersect);
    printf("\t[%s] n_squares should be 2 : %d\n", __FUNCTION__, n_squares);
    assert(n_squares == 2);
    _fb_window_t area_to_intersect_2 = {500, 300, 200, 120};
    n_squares = _fb_get_rectangles(rectangles, &main_window, &area_to_intersect_2);
    printf("\t[%s]: area_to_intersect values: x: %d - y: %d - w: %d - h: %d\n", __FUNCTION__, area_to_intersect_2.x_orig, area_to_intersect_2.y_orig, area_to_intersect_2.width, area_to_intersect_2.height);
    printf("\t[%s]: n_squares should be 4: %d\n", __FUNCTION__, n_squares);
    for(int i = 0; i < n_squares; i++) {
        printf("\t[%s]: x_orig: %d - y_orig: %d - width: %d - height: %d\n", __FUNCTION__, rectangles[i].x_orig, rectangles[i].y_orig, rectangles[i].width, rectangles[i].height);
    }
    _fb_window_t area_to_intersect_3 = {0, 0, 200, 120};
    n_squares = _fb_get_rectangles(rectangles, &main_window, &area_to_intersect_3);
    printf("\t[%s]: area_to_intersect values: x: %d - y: %d - w: %d - h: %d\n", __FUNCTION__, area_to_intersect_3.x_orig, area_to_intersect_3.y_orig, area_to_intersect_3.width, area_to_intersect_3.height);
    printf("\t[%s]: n_squares should be 4: %d\n", __FUNCTION__, n_squares);
    for(int i = 0; i < n_squares; i++) {
        printf("\t[%s]: x_orig: %d - y_orig: %d - width: %d - height: %d\n", __FUNCTION__, rectangles[i].x_orig, rectangles[i].y_orig, rectangles[i].width, rectangles[i].height);
    }
}
