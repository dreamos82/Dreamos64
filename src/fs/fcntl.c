#include <fcntl.h>
#include <logging.h>
#include <task.h>
#include <string.h>
#include <task.h>
#include <ustar.h>
#include <vfs.h>


int open(const char *path, int flags) {
    //syscall(....)
    uint64_t new_fd = task_alloc_fd();
    pretty_logf(Verbose, "New Fd: %d", new_fd);
    return vfs_open(path, flags);
}


