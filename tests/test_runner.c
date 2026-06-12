#include "include/test_stats.h"
#include <test_stats.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

test_global_stats_t total_stats;

int main() {
    int pipe_fd[2];
	pipe(pipe_fd);
	pid_t pid = fork();
    if (pid == 0) {
        char fd_str[10];
		sprintf(fd_str, "%d", pipe_fd[1]);
		printf("Launching tests as child\n");
        close(pipe_fd[0]);
        execl("./test_vfs.o",	"test_vfs.o", fd_str, NULL);
    } else {
        close(pipe_fd[1]);
        test_stats_t stats;
        read(pipe_fd[0], &stats, sizeof(test_stats_t));
        printf("module id: %d\n", stats.module_id);
        printf("Test name: %s\n", stats.module_title);
        printf("Passed:: %d\n", stats.tests_passed);
        printf("Failed:: %d\n", stats.tests_failed);
    }
}
