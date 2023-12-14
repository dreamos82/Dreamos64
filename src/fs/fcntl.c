#include <fcntl.h>
#include <logging.h>
#include <string.h>
#include <vfs.h>

vfs_file_descriptor_t vfs_opened_files[OPENEDFILES_MAX];

int open(const char *path, int flags) {
    pretty_logf(Verbose, "(open) Try to open file: %s", path);
    int mountpoint_id = get_mountpoint_id(path);
    if (mountpoint_id < 0) {
        return -1;
    }

    pretty_logf(Verbose, "(open) --- mountpoint id for file: %d and flags: %d ", mountpoint_id, flags);
    mountpoint_t mountpoint = mountpoints[mountpoint_id];
    pretty_logf(Verbose, "(open) --- mountpoint id for file: %s", mountpoint.mountpoint);
    char *relative_path = get_relative_path(mountpoint.mountpoint, path);
    pretty_logf(Verbose, "(open) --- relative path is: %s", relative_path);
    if (mountpoint.file_operations.open == NULL) {
        return -1;
    }

    int driver_fd = mountpoint.file_operations.open(relative_path, flags);
    if (driver_fd < 0) {
        return -1;
    }

    if ( vfs_fd_index > OPENEDFILES_MAX ) {
        return -1;
    }

    vfs_opened_files[vfs_fd_index].fs_specific_id = driver_fd;
    vfs_opened_files[vfs_fd_index].mountpoint_id = mountpoint_id;
    vfs_opened_files[vfs_fd_index].buffer_read_pos = 0;
    strcpy(vfs_opened_files[vfs_fd_index].filename, path);
    return vfs_fd_index++;
}
