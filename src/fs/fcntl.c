#include <fcntl.h>
#include <logging.h>
#include <string.h>
#include <ustar.h>
#include <vfs.h>


int open(const char *path, int flags) {
    //syscall(....)
    return vfs_open(path, flags);
}

int vfs_open(const char *path, int flags) {
    pretty_logf(Verbose, "Try to open file: %s", path);
    // In future if the vnode for that file already exists, it would be returned, and passed to vfs_lookup.
    unsigned int cur_node_index = vnode_index;
    int vnode_id;
    vnode_t *vnode = vnode_get_next_free(&vnode_id);
    if ( vnode == NULL ) {
        pretty_log(Fatal, "Error cannot find vnode");
        return -1;
    }

    int result = vfs_lookup(path,flags, vnode);
    pretty_logf(Verbose, "file Size: %d", vnode->size);
    if (result == 0) {
        pretty_logf(Verbose, "file Size: %d", vnode_cache[cur_node_index].size);
        return vnode_id;
    }
}

