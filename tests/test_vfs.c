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
unsigned int number_of_tests = 0;

int main(int argc, char **argv) {
    int pipe_fd = -1;
    if (argc > 1) {
            //pipe_fd = atoi(argv[1]);
            has_pipe = true;
            pipe_fd = atoi(argv[1]);
            printf("Found pipe id: %d\n", pipe_fd);
    }
    prepare_tests(pipe_fd);
    tests[0].handler(0);
    print_stats(tests[0].stats);
    if ( has_pipe ) {
        write(pipe_fd, &number_of_tests, sizeof(number_of_tests)); 
        send_stats(pipe_fd, tests[0].stats);
        close(pipe_fd);
    }

}

void prepare_tests(){
    vfs_init();
    printf("Testing Virtual File System functions\n");
    printf("===============================\n\n");
    test_init(1, &tests);
    add_test(1, "Testing Virtual File System Fucntions", test_get_mountpoint_id, tests);
    number_of_tests++;
}

void test_get_mountpoint_id(unsigned int id) {
    printf("%d) %s:\n", tests[id].stats.module_id, tests[id].stats.module_title);
    int last = vfs_get_mountpoint_id("/home/dreamos82", &vnode);    
    pretty_assert_stat(last, positions[0], ==, tests[id].stats, "(test_get_mountpoint_id): Testing path /home/dreamos82", has_pipe);
    last = vfs_get_mountpoint_id("/home/mount/dreamos82", &vnode);        
    pretty_assert_stat(last, positions[1], ==, tests[id].stats, "(test_get_mountpoint_id): Testing path /home/mount/dreamos82", has_pipe);
    last = vfs_get_mountpoint_id("/usr", &vnode);    
    pretty_assert_stat(last, positions[2], ==, tests[id].stats, "(test_get_mountpoint_id): Testing /usr", has_pipe);
    last = vfs_get_mountpoint_id("/", &vnode);
    pretty_assert_stat(last, positions[3], ==, tests[id].stats,"(test_get_mountpoint_id): Testing /", has_pipe);
    last = vfs_get_mountpoint_id("/usr/asd", &vnode);
    pretty_assert_stat(last, positions[4], ==, tests[id].stats, "(test_get_mountpoint_id): Testing /usr/asd", has_pipe);
    //printf("\tStats: module_id: %d, Title: %s, Tests Passed: %d\n", tests[id].stats.module_id, tests[id].stats.module_title, tests[id].stats.tests_passed);
}
