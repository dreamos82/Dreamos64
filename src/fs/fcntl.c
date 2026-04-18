#include <fcntl.h>
#include <logging.h>
#include <string.h>
#include <ustar.h>
#include <vfs.h>


int open(const char *path, int flags) {
    //syscall(....)
    return vfs_open(path, flags);
}


