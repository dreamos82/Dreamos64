#include <logging.h>
#include <sys_open.h>

int sys_open(const char *path, int flags) {
    pretty_logf(Verbose, "Path to open: %s - Flags: %x", path, flags);
    return 0;
}
