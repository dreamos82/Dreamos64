#include <test_stats.h>
#include <unistd.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>


void test_init(unsigned int no_modules, test_runner_t **tests){
    *tests = (test_runner_t *) malloc(no_modules * sizeof(test_runner_t*));
    printf("Initializing no. modules: %d...", no_modules);
    for (int i = 0; i < no_modules; i++) {
        tests[i] = (test_runner_t*) malloc(sizeof(test_runner_t));
        tests[i]->stats.tests_passed = 0;
        tests[i]->stats.tests_failed = 0;        
        tests[i]->stats.module_id = 0;
        tests[i]->handler = NULL;        
    }
    printf("Done\n");
}

void add_test(unsigned int module_idx, char *module_title, void (*handler)(unsigned int), test_runner_t *tests) {
    printf("(%d) Adding Test: %s\n", module_idx, module_title);
    strcpy(tests[module_idx-1].stats.module_title, module_title);
    tests[module_idx-1].stats.module_id = module_idx;
    //printf("%s\n", tests[module_idx-1].stats.module_title);    
    tests[module_idx-1].handler = handler;    
    tests[module_idx-1].stats.tests_passed = 0;
}

void send_stats(int fd, test_stats_t stats) { 
    if (fd > 0) {
        write(fd, &stats, sizeof(test_stats_t));
    }
}

void update_global_stats(test_global_stats_t *global_stats, test_stats_t cur_stats) {
    global_stats->stats.tests_passed += cur_stats.tests_passed;
    global_stats->stats.tests_failed += cur_stats.tests_failed;
    global_stats->total_modules++;
}


void print_stats(test_stats_t stats) {
    printf("\tStats: module_id: %d, Title: %s, Tests Passed: %d - Tests failed: %d\n", stats.module_id, stats.module_title, stats.tests_passed, stats.tests_failed);
}
