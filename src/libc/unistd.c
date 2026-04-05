#include <unistd.h>
#include <vfs.h>
#include <logging.h>

int close (int fildes) {
    if (fildes >= 0 && fildes < OPENEDFILES_MAX) {
        //TODO: it should check for opened files by all threads.
        //and do error checking for mountpoint close.
        pretty_logf(Verbose, "called with fildes: %d", fildes);
        if (vnode_cache[fildes].vfs_root != NULL) {
            mountpoint_t *mountpoint = vnode_cache[fildes].vfs_root;
            if (mountpoint->file_operations.close != NULL) {
                //TODO: should pass a vnode to the close operation
                mountpoint->file_operations.close(fildes);
            }
        }        
        pretty_logf(Verbose, "File to close size: %d", vnode_cache[fildes].size);
        vnode_clear(&vnode_cache[fildes]);
        return 0;
    }
    return -1;
}

ssize_t read(int fildes, void *buf, size_t nbytes){
    //Will be implemented in the future
    if (fildes >= 0 && fildes < OPENEDFILES_MAX) {
        vnode_t vnode = vnode_cache[fildes];
        ssize_t bytes_read = vfs_read(&vnode, buf, 0, nbytes);
        return bytes_read;
        //vnode.read();
        /*if (vfs_opened_files[fildes].fs_specific_id >= 0) {
            int fs_specific_id = vfs_opened_files[fildes].fs_specific_id;
            int mountpoint_id = vfs_opened_files[fildes].mountpoint_id;
            mountpoint_t mountpoint = mountpoints[mountpoint_id];
            if (mountpoint.file_operations.read != NULL) {
                return mountpoint.file_operations.read(&vnode, fs_specific_id, buf, nbytes);
            }
            return 0;
        }*/
    }
    return -1;
}
