#include <logging.h>
#include <vnode.h>
#include <vfs.h>

vnode_t vnode_cache[VNODE_OPENED_MAX];

vnode_t* vnode_get_next_free() {
    if ( vnode_index < OPENEDFILES_MAX ) {
        vnode_cache[vnode_index].size = 0;
        vnode_cache[vnode_index].refcount = 0;
        vnode_cache[vnode_index].v_type = 0;
        vnode_cache[vnode_index].v_data = 0;        
        return &vnode_cache[vnode_index++];
    }
    return NULL;
}

