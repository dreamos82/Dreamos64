#include <ustar.h>
#include <logging.h>

int ustar_open(char *path, int flags) {
    loglinef(Verbose, "(ustar_open) called with path: %s and flags: %d", path, flags);
    return 3;
}

int ustar_close(int ustar_fildes) {
    loglinef(Verbose, "(ustar_close) called with fildes: %d", ustar_close);
    return 0;
}
