#ifndef __VNODE_H
#define __VNODE_H

#include <stdint.h>
#include <vfs.h>

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
    void *v_data; // This will contains the data from the FS
};

typedef struct vnode_t vnode_t;
#endif
