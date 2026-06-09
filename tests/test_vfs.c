#include <vfs.h>
#include <assert.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <test_common.h>
#include <test_stats.h>
#include <test_vfs.h>
#include <unistd.h>

int positions[5] = { 0, 1, 2, 0, 2 };
vnode_t vnode;
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
    prepare_tests(pipe_fd);    
    //test_get_mountpoint_id();
}

void prepare_tests(int pipe_fd){
    vfs_init();
    printf("Testing Virtual File System functions\n");
    printf("===============================\n\n");
    test_init(1, &tests);
    add_test(1, "Testing Virtual File System Fucntions", test_get_mountpoint_id, tests);
    tests[0].handler(pipe_fd);
}

void test_get_mountpoint_id(int pipe_fd) {
    printf("Testing vfs functions -\n");
    int last = vfs_get_mountpoint_id("/home/dreamos82", &vnode);    
    pretty_assert_stat(last, positions[0], ==, tests[0].stats, "(test_get_mountpoint_id): Testing path /home/dreamos82");
    pretty_assert(positions[0], last, ==, "(test_get_mountpoint_id): Testing path /home/dreamos82");
    printf("\tstats: %d\n", tests[0].stats.tests_passed);
    last = vfs_get_mountpoint_id("/home/mount/dreamos82", &vnode);        
    pretty_assert_stat(last, positions[1], ==, tests[0].stats, "(test_get_mountpoint_id): Testing path /home/mount/dreamos82");
    pretty_assert(last, positions[1], ==, "(test_get_mountpoint_id): Testing path /home/mount/dreamos82");
    last = vfs_get_mountpoint_id("/usr", &vnode);    
    pretty_assert_stat(last, positions[2], ==, tests[0].stats, "(test_get_mountpoint_id): Testing /usr");
    pretty_assert(last, positions[2], ==, "(test_get_mountpoint_id): Testing /usr");
    last = vfs_get_mountpoint_id("/", &vnode);
    pretty_assert_stat(last, positions[3], ==, tests[0].stats,"(test_get_mountpoint_id): Testing /");
    pretty_assert(last, positions[3], ==, "(test_get_mountpoint_id): Testing /");
    last = vfs_get_mountpoint_id("/usr/asd", &vnode);
    pretty_assert_stat(last, positions[4], ==, tests[0].stats, "(test_get_mountpoint_id): Testing /usr/asd");
    pretty_assert(last, positions[4], ==, "(test_get_mountpoint_id): Testing /usr/asd");
    printf("\tStats: module_id: %d, Title: %s, Tests Passed: %d\n", tests[0].stats.module_id, tests[0].stats.module_title, tests[0].stats.tests_passed);
    if ( has_pipe ) {
        send_stats(pipe_fd, tests[0].stats);
        close(pipe_fd);
    }
}
