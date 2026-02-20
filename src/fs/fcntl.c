#include <fcntl.h>
#include <logging.h>
#include <string.h>
#include <ustar.h>
#include <vfs.h>

vfs_file_descriptor_t vfs_opened_files[OPENEDFILES_MAX];


int open(const char *path, int flags) {
    return vfs_open(path, flags);
}

int vfs_open(const char *path, int flags) {
    pretty_logf(Verbose, "Try to open file: %s", path);
    // In future if the vnode for that file already exists, it would be returned, and passed to vfs_lookup.
    vnode_t *vnode = vnode_get_next_free();
    if ( vnode == NULL) {
        pretty_log(Fatal, "Error cannot find vnode");
        return -1;
    }

    int result = vfs_lookup(path,flags, vnode);
    return result;
}
