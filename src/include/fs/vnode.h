#ifndef __VNODE_H
#define __VNODE_H

#include <sys/types.h>

#define VNODE_OPENED_MAX 5

//Forward declarations
struct mountpoint_t;
typedef struct mountpoint_t mountpoint_t;

enum vnode_types {
    V_NONE,
    V_REGULAR_FILE,
    V_DIR,
    V_BAD
};

typedef enum vnode_types vnode_types;

struct vnode_t {
    ssize_t size; // Size of the file
    int refcount; // Number of processes pointing here
    vnode_types v_type;
    mountpoint_t *vfs_root;
    void *v_data; // This will contains the data from the FS    
};

typedef struct vnode_t vnode_t;

extern vnode_t vnode_cache[VNODE_OPENED_MAX];

vnode_t *vnode_get_next_free();
#endif
    
