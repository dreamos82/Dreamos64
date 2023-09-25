#include <ustar.h>
#include <logging.h>
#include <string.h>

int ustar_open(const char *path, int flags, ...) {
    loglinef(Verbose, "(ustar_open) called with path: %s and flags: %d", path, flags);
    return 3;
}

int ustar_close(int ustar_fildes) {
    loglinef(Verbose, "(ustar_close) called with fildes: %d", ustar_fildes);
    return 0;
}

ssize_t ustar_read(int ustar_fildes, char *buf, size_t nbytes) {
    (void)ustar_fildes;
    (void)nbytes;
    strcpy(buf, "Test string");
    return 12;
} 
