#ifndef __TEST_STATS__
#define __TEST_STATS__

typedef struct {
    unsigned int module_id;
    unsigned int tests_passed;
    unsigned int tests_failed;
    
    char module_title[256];    
} test_stats_t;

typedef struct {
    test_stats_t stats;
    void (*handler)(int);
    
} test_runner_t;

//TODO: add a new struct with the handler and tests_stat_t and the runner. 
// The tests will be launched by the child process.

void test_init(unsigned int no_modules, test_runner_t **tests);
void add_test(unsigned int module_idx, char *module_title, void (*handler)(int), test_runner_t *tests);
void send_stats(int fd, test_stats_t stats);

#define pretty_assert_stat(expected_value, returned_value, comparator, stats, msg) \
    ({ \
        printf("\t(%s) %s expected_value: %ld returned value:  %ld\n", __FUNCTION__, msg, expected_value, returned_value); \
        if (expected_value comparator returned_value) { \
            stats.tests_passed++; \
        } else { \
            stats.tests_failed++; \
        } \
    })

#endif
