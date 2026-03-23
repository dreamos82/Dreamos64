#include <logging.h>
#include <vnode.h>
#include <vfs.h>

vnode_t vnode_cache[VNODE_OPENED_MAX];
vfs_file_descriptor_t vfs_opened_files[OPENEDFILES_MAX];

vnode_t* vnode_get_next_free(int *id_to_return) {
    // This function currently just get the next free node index, and clean it's content.
    pretty_logf(Verbose, "vnode_index: %d", vnode_index);
    if ( vnode_index < OPENEDFILES_MAX ) {
        vnode_cache[vnode_index].size = 0;
        vnode_cache[vnode_index].refcount = 0;
        vnode_cache[vnode_index].v_type = 0;
        vnode_cache[vnode_index].v_data = 0;
        *id_to_return = vnode_index;
        return &vnode_cache[vnode_index++];
    }
    return NULL;
}

void vnode_clear(vnode_t *vnode) {
    //For now is an array so we are going just to set all values to 0
        vnode->size = 0;
        vnode->refcount = 0;
        vnode->v_type = 0;
        vnode->v_data = 0;
}
