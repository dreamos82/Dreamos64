#include "include/test_stats.h"
#include <test_stats.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

#define TESTS_FILE "tests"

//gcc -I include test_runner.c -o tests_runner.o

unsigned int get_number_of_tests(FILE *fp);
unsigned int get_tests(FILE *fp, unsigned int no_lines, char **tests);
void update_total_stats(test_stats_t stats);

test_global_stats_t total_stats;
test_stats_t *tests_stats;
int main() {
    int pipe_fd[2];
	pipe(pipe_fd);	
    FILE *fp = fopen(TESTS_FILE, "r");
    char **tests;
    if (fp == NULL) {
        printf("FATAL: can't open tests file, make sure to have one in the current directory\n");
        return -1;
    }
    unsigned int no_lines = get_number_of_tests(fp);
    if (no_lines <=0) {
        printf("FATAL: can't open tests file, make sure to have one in the current directory\n");
        return -1;
    }
    tests = malloc(no_lines * sizeof(char *));
    tests_stats = malloc(no_lines * sizeof(test_stats_t));
    get_tests(fp, no_lines, tests);
    /*strcpy(tests[0], "./test_vfs.o");    
    strcpy(tests[1], "./test_tar.o");
    strcpy(tests[2], "./test_utils.o");*/
    bool isForked = false;
    for (int i=0; i < no_lines; i++) {
        printf("Launching tests as child %s\n", tests[i]);
        pid_t pid = fork();        
        if (pid == 0) {
            // The child process will only write to the pipe, so I can close the read channel.
            isForked = true;
            char fd_str[10];
            sprintf(fd_str, "%d", pipe_fd[1]);
            close(pipe_fd[0]);
            execl(tests[i],	tests[i], fd_str, NULL);
        }
        int status = 0;
        while (waitpid(pid, &status, 0) < 0);
    }
    //pid_t pid = fork();
    /*if (pid == 0) {
        // The child process will only write to the pipe, so I can close the read channel.
        char fd_str[10];
		sprintf(fd_str, "%d", pipe_fd[1]);
		printf("Launching tests as child\n");
        close(pipe_fd[0]);
        execl("./test_vfs.o",	"test_vfs.o", fd_str, NULL);
    } else {*/
        // The parent process will only read from the pipe, so I can close the write channel.
    //if (pid != 0) {
    total_stats.total_modules = 0;
    total_stats.stats.module_id = 0;
    total_stats.stats.tests_passed = 0;
    total_stats.stats.tests_failed = 0;
    if (!isForked) {
        close(pipe_fd[1]);
        for (int i=0; i<no_lines; i++) {
            test_stats_t stats;        
            unsigned int no_tests = 0;
            read(pipe_fd[0], &no_tests, sizeof(no_tests));
            printf("Number of tests received: %d\n", no_tests);
            for(int i=0; i < no_tests; i++) {
                read(pipe_fd[0], &stats, sizeof(test_stats_t));
                printf("module id: %d\n", stats.module_id);
                printf("Test name: %s\n", stats.module_title);
                printf("\tPassed:: %d\n", stats.tests_passed);
                printf("\tFailed:: %d\n\n", stats.tests_failed);
                update_total_stats(stats);
            }
            printf("=================\n");
        }
        printf("Total stas: \n");
        printf("\t Tests_passed: %d\n", total_stats.stats.tests_passed);
        printf("\t Tests_failed: %d\n", total_stats.stats.tests_failed);
        printf("\t Total tests: %d\n", total_stats.stats.tests_passed + total_stats.stats.tests_failed);
        
    }
}

void update_total_stats(test_stats_t stats) {
    total_stats.stats.tests_passed += stats.tests_passed;
    total_stats.stats.tests_failed += stats.tests_failed;
    total_stats.total_modules++;
}

unsigned int get_number_of_tests(FILE *fp) {
    int counter = 0;
    char line[256];
    while (fgets(line, sizeof(line), fp)) {
        counter++;        
    }
    return counter;
}

unsigned int get_tests(FILE *fp, unsigned int no_lines, char **tests) {
    rewind(fp);
    for (int i=0; i < no_lines; i++) {    
        tests[i] = (char *) malloc(32);
        fgets(tests[i], 32, fp);
        tests[i][strcspn(tests[i], "\n")] = '\0';
    }
    return 0;
}
