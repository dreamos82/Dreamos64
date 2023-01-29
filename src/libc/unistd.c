#include <unistd.h>
#include <vfs.h>
#include <logging.h>

int close (int fildes) {
    if (fildes >= 0 && fildes < OPENEDFILES_MAX) {
        //TODO: it should check for opened files by all threads.
        loglinef(Verbose, "(close) called with fildes: %d", fildes);       
        if (vfs_opened_files[fildes].fs_specific_id > 0) {
            int fs_specific_id = vfs_opened_files[fildes].fs_specific_id;
            int mountpoint_id = vfs_opened_files[fildes].mountpoint_id;
            mountpoint_t mountpoint = mountpoints[mountpoint_id];
            if (mountpoint.file_operations.close != NULL) {
                mountpoint.file_operations.close(fs_specific_id);
            }
        }
        vfs_opened_files[fildes].fs_specific_id = -1;
        vfs_opened_files[fildes].mountpoint_id = -1;
        vfs_opened_files[fildes].buffer_read_pos = -1;
        return 0;
    }
    return -1;
}

ssize_t read(int fildes, void *buf, size_t nbytes){
    //Will be implemented in the future
    if (fildes >= 0 && fildes < OPENEDFILES_MAX) {
        if (vfs_opened_files[fildes].fs_specific_id > 0) {
            int fs_specific_id = vfs_opened_files[fildes].fs_specific_id;
            int mountpoint_id = vfs_opened_files[fildes].mountpoint_id;
            mountpoint_t mountpoint = mountpoints[mountpoint_id];
            if (mountpoint.file_operations.read != NULL) {
                mountpoint.file_operations.read(fs_specific_id, buf, nbytes);
            }
            return nbytes;
        }
    }
    return -1;
}

