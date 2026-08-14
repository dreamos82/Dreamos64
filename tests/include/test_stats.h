#ifndef __TEST_STATS__
#define __TEST_STATS__

#include <assert.h>

typedef struct {
    unsigned int module_id;
    unsigned int tests_passed;
    unsigned int tests_failed;
    
    char module_title[256];    
} test_stats_t;

typedef struct {
    test_stats_t stats;
    void (*handler)(unsigned int);
} test_runner_t;

typedef struct {
    test_stats_t stats;
    unsigned int total_modules;
    test_stats_t *module_stats;
}test_global_stats_t;

//TODO: add a new struct with the handler and tests_stat_t and the runner. 
// The tests will be launched by the child process.

void test_init(unsigned int no_modules, test_runner_t **tests);
void add_test(unsigned int module_idx, char *module_title, void (*handler)(unsigned int), test_runner_t *tests);
void send_stats(int fd, test_stats_t stats);
void update_global_stats(test_global_stats_t *global_stats, test_stats_t cur_stats);
void print_stats(test_stats_t stats);

#define pretty_assert_stat(expected_value, returned_value, comparator, stats, msg, continue_on_fail) \
    ({ \
        printf("\t(%s) %s expected_value: %ld returned value:  %ld\n", __FUNCTION__, msg, expected_value, returned_value); \
        if (!continue_on_fail) { \
            assert(expected_value comparator returned_value); \
        } else if (expected_value comparator returned_value) { \
            stats.tests_passed++; \
        } else { \
            stats.tests_failed++; \
        } \
    })

#endif
