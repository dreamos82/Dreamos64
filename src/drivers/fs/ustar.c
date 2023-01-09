#include <ustar.h>
#include <logging.h>

int ustar_open(char *path, int flags) {
    loglinef(Verbose, "ustar_open_called with path: %s and flags: %d", path, flags);
    return 3;
}
