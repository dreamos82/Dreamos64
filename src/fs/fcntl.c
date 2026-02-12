#include <fcntl.h>
#include <logging.h>
#include <string.h>
#include <vfs.h>

vfs_file_descriptor_t vfs_opened_files[OPENEDFILES_MAX];


int open(const char *path, int flags) {
    return vfs_open(path, flags);
}

int vfs_open(const char *path, int flags) {
    pretty_logf(Verbose, "Try to open file: %s", path);
    vnode_t *vnode = vnode_get_next_free();
    if ( vnode != NULL) {
        pretty_log(Fatal, "Error cannot find vnode");
        return -1;
    }
    return vfs_lookup(path,flags, vnode);
}
