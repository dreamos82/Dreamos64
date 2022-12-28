#include <vfs.h>
#include <fcntl.h>
#include <logging.h>
#include <string.h>

vfs_file_descriptor_t vfs_opened_files[OPENEDFILES_MAX];

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
            int driver_fd = mountpoint.file_operations.open(relative_path, flags);
            if ( driver_fd >= 0) {
                if ( vfs_fd_index < OPENEDFILES_MAX ) {                    
                    vfs_opened_files[vfs_fd_index].fs_specific_id = driver_fd;
                    vfs_opened_files[vfs_fd_index].mountpoint_id = mountpoint_id;
                    vfs_opened_files[vfs_fd_index].buffer_read_pos = 0;
                    strcpy(vfs_opened_files[vfs_fd_index].filename, path);
                    return vfs_fd_index++;
                }
                return -1;
            }
        return -1;
        }
    }
    return -1;
}
