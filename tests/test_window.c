#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <test_common.h>
#include <test_stats.h>
#include <unistd.h>
#include <window.h>

void test_get_rectangles(unsigned int id);
void prepare_tests();

unsigned int number_of_tests = 0;
test_runner_t *tests;
bool has_pipe = false;

int main(int argc, char **argv) {
    int pipe_fd = -1;
    if (argc > 1) {
            //pipe_fd = atoi(argv[1]);
            has_pipe = true;
            pipe_fd = atoi(argv[1]);
            printf("Found pipe id: %d\n", pipe_fd);
    }
    prepare_tests();
    printf("%d) %s:\n", tests[0].stats.module_id, tests[0].stats.module_title);
    tests[0].handler(0);
    print_stats(tests[0].stats);
     if ( has_pipe ) {
        write(pipe_fd, &number_of_tests, sizeof(number_of_tests));
        send_stats(pipe_fd, tests[0].stats);
        close(pipe_fd);
    }
    printf("\n");
}

void prepare_tests(){
    printf("Testing Window function  -\n");
    printf("===============================\n\n");
    number_of_tests = 0;
    test_init(1, &tests);
    add_test(1, "Testing get rectangles function", test_get_rectangles, tests);
    number_of_tests++;
}

void test_get_rectangles(unsigned int id) {
    _fb_window_t rectangles[4];
    _fb_window_t main_window = {0,0, 1280, 800};
    _fb_window_t area_to_intersect = {1030, 0, 250, 250};
    printf("Testing _fb_get_rectangles function\n");
    printf("\t[%s]: main_window values: x: %d - y: %d - w: %d - h: %d\n", __FUNCTION__, main_window.x_orig, main_window.y_orig, main_window.width, main_window.height);
    uint8_t n_squares = _fb_get_rectangles(rectangles, &main_window, NULL);
    pretty_assert_stat(0, n_squares, ==, tests[id].stats, "Testing number of rectangles returned. Passing NULL", has_pipe);
    n_squares = _fb_get_rectangles(rectangles, &main_window, &area_to_intersect);
    pretty_assert_stat(2, n_squares, ==, tests[id].stats, "Testing number of rectangles returned.", has_pipe);
    pretty_assert_stat( 0, rectangles[0].x_orig, ==, tests[id].stats, "Testing rectangles[0].x_orig", has_pipe);
    pretty_assert_stat( 0, rectangles[0].y_orig, ==, tests[id].stats, "Testing rectangles[0].y_orig", has_pipe);
    pretty_assert_stat( 1030, rectangles[0].width, ==, tests[id].stats, "Testing rectangles[0].width", has_pipe);
    pretty_assert_stat( 800, rectangles[0].height, ==, tests[id].stats, "Testing rectangles[0].height", has_pipe);
    pretty_assert_stat( 1030, rectangles[1].x_orig, ==, tests[id].stats, "Testing rectangles[1].x_orig", has_pipe);
    pretty_assert_stat( 250, rectangles[1].y_orig, ==, tests[id].stats, "Testing rectangles[1].y_orig", has_pipe);
    pretty_assert_stat( 250, rectangles[1].width, ==, tests[id].stats, "Testing rectangles[1].width", has_pipe);
    pretty_assert_stat( 550, rectangles[1].height, ==, tests[id].stats, "Testing rectangles[1].height", has_pipe);
    _fb_window_t area_to_intersect_2 = {500, 300, 200, 120};
    n_squares = _fb_get_rectangles(rectangles, &main_window, &area_to_intersect_2);
    printf("\t[%s]: area_to_intersect values: x: %d - y: %d - w: %d - h: %d\n", __FUNCTION__, area_to_intersect_2.x_orig, area_to_intersect_2.y_orig, area_to_intersect_2.width,
    area_to_intersect_2.height);
    //pretty_assert(4, n_squares, ==, "Testing number of rectangles returned.");
    pretty_assert_stat(4, n_squares, ==, tests[id].stats, "Testing number of rectangles returned.", has_pipe);
    _fb_window_t four_results[4] = { {0, 0, 500, 800}, {700, 0, 580, 800 }, {500, 0, 200, 300}, {500, 420, 200, 380} };
    for(int i = 0; i < n_squares; i++) {
        printf("\t[%s]: item: %d) x_orig: %d - y_orig: %d - width: %d - height: %d\n", __FUNCTION__, i, rectangles[i].x_orig, rectangles[i].y_orig, rectangles[i].width, rectangles[i].height);
        pretty_assert_stat( four_results[i].x_orig, rectangles[i].x_orig, ==, tests[id].stats, "Testing rectangles[i].x_orig", has_pipe);
        pretty_assert_stat( four_results[i].y_orig, rectangles[i].y_orig, ==, tests[id].stats, "Testing rectangles[i].y_orig", has_pipe);
        pretty_assert_stat( four_results[i].width, rectangles[i].width, ==, tests[id].stats, "Testing rectangles[i].width"   , has_pipe);
        pretty_assert_stat( four_results[i].height, rectangles[i].height, ==, tests[id].stats, "Testing rectangles[i].height", has_pipe);
    }
    _fb_window_t area_to_intersect_3 = {0, 0, 200, 120};
    n_squares = _fb_get_rectangles(rectangles, &main_window, &area_to_intersect_3);
    //pretty_assert(2, n_squares, ==, "Testing number of rectangles returned.");
    pretty_assert_stat(2, n_squares, ==, tests[id].stats, "Testing number of rectangles returned.", has_pipe);
    _fb_window_t four_results_second[2] = { {200, 0, 1080, 800}, {0, 120, 200, 680 }};
    for(int i = 0; i < n_squares; i++) {
        printf("\t[%s]: item: %d) x_orig: %d - y_orig: %d - width: %d - height: %d\n", __FUNCTION__, i, rectangles[i].x_orig, rectangles[i].y_orig, rectangles[i].width, rectangles[i].height);
        pretty_assert_stat( four_results_second[i].x_orig, rectangles[i].x_orig, ==, tests[id].stats, "Testing rectangles[i].x_orig", has_pipe);
        pretty_assert_stat( four_results_second[i].y_orig, rectangles[i].y_orig, ==, tests[id].stats, "Testing rectangles[i].y_orig", has_pipe);
        pretty_assert_stat( four_results_second[i].width,  rectangles[i].width,  ==, tests[id].stats, "Testing rectangles[i].width"   , has_pipe);
        pretty_assert_stat( four_results_second[i].height, rectangles[i].height, ==, tests[id].stats, "Testing rectangles[i].height", has_pipe);
    }
}
