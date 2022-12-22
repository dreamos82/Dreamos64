#include <fcntl.h>
#include <vfs.h>
#include <logging.h>


int open(const char *path, int flags) {
    loglinef(Verbose, "Try to open file: %s", path);
    int mountpoint_id = get_mountpoint_id(path);
    if (mountpoint_id > -1) {
        loglinef(Verbose, "--- mountpoint id for file: %d and flags: %d ", mountpoint_id, flags);
        mountpoint_t mountpoint = mountpoints[mountpoint_id];
        loglinef(Verbose, "--- mountpoint id for file: %s", mountpoint.mountpoint);
        char *relative_path = get_relative_path(mountpoint.mountpoint, path);
        loglinef(Verbose, "--- relative path is: %s", relative_path);
        if (mountpoint.file_operations.open != NULL) {
            mountpoint.file_operations.open(relative_path, flags);
        }
    } else {
        logline(Verbose, "Error");
    }
    return 0;
}
