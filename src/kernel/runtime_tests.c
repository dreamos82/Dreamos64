#include <fcntl.h>
#include <logging.h>
#include <runtime_tests.h>
#include <spinlock.h>
#include <task.h>
#include <thread.h>
#include <unistd.h>
#include <vfs.h>

spinlock_t* test_lock;
char b = 'r';
char c = 's';

void execute_runtime_tests() {
    //TODO create threads

    test_lock = spinlock_init();
    create_thread("spinlock_1", NULL, test_spinlock, &b, false);
    create_thread("spinlock_2", NULL, test_spinlock, &c, false);
    spinlock_free(test_lock);

}

void test_spinlock(char *c) {
    spinlock_acquire(&(test_lock->locked));
    loglinef(Verbose, " [Spinlock_test] I am thread: %c", *c);
    thread_sleep(1000);
    spinlock_release(&(test_lock->locked));
    thread_sleep(500);
    loglinef(Verbose, " [Spinlock_test] I am thread: %c", *c);
    thread_sleep(1000);
    spinlock_release(&(test_lock->locked));
}

void test_get_task() {
    loglinef(Verbose, "Testing get_task");
    task_t* found_task = get_task(4);
    if (found_task == NULL) {
        logline(Verbose,"Task 4 is NULL");
    } else {
        logline(Verbose, "Something wrong");
    };

    found_task = get_task(2);
    if(found_task != NULL) {
        loglinef(Verbose, "Task found: %d - Name: %s", found_task->task_id, found_task->task_name);
    } else {
        logline(Verbose, "Something wrong");
    }

}

void test_vfs() {
    int fd_id = open("/home/ivan/testfile.txt", 0);
    loglinef(Verbose, "(kernel_main) Obtained fd id: %d fs_fd_id: %d", fd_id, vfs_opened_files[fd_id].fs_specific_id);
    char buffer[15] = "";
    read(fd_id, buffer, 15);
    loglinef(Verbose, "(kernel_main) Output of read: %s", buffer);
    int result = close(fd_id);
    loglinef(Verbose, "(kernel_main) Closing file with id: %d", result);
}
