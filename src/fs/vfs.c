#include <logging.h>
#include <string.h>
#include <ustar.h>
#include <vfs.h>

mountpoint_t mountpoints[MOUNTPOINTS_MAX];
unsigned int vnode_index;
unsigned int mountpoint_index;

mountpoint_t *mountpoints_list;


void vfs_init() {
    pretty_log(Verbose, "Initializing VFS layer");
    for (int i=0; i < MOUNTPOINTS_MAX; i++) {
        strcpy(mountpoints[i].name, "");
        strcpy(mountpoints[i].mountpoint, "");
        mountpoints[i].file_operations.open = NULL;
        mountpoints[i].file_operations.close = NULL;
    }

    // The first item will always be the root!
    strcpy(mountpoints[0].name, "ArrayFS");
    strcpy(mountpoints[0].mountpoint, "/");
    // Adding some fake fs
    strcpy(mountpoints[1].name, "ArrayFS");
    strcpy(mountpoints[1].mountpoint, "/home/mount");
    // Adding some fake fs
    strcpy(mountpoints[2].name, "ArrayFS");
    strcpy(mountpoints[2].mountpoint, "/usr");
    // Adding some fake fs
    strcpy(mountpoints[3].name, "ustar");
    strcpy(mountpoints[3].mountpoint, "/external");
    mountpoints[3].file_operations.open = ustar_open;
    mountpoints[3].file_operations.close = ustar_close;
    mountpoints[3].file_operations.read = ustar_read;
    mountpoints[3].vnode_operations.lookup = ustar_lookup;
    mountpoint_index=3;
    vnode_index = 0;
}

int vfs_register(char *file_system_name, char *mountpoint, fs_file_operations_t file_operations){
    if ( mountpoint_index >MOUNTPOINTS_MAX )
        return -1;
    strcpy(mountpoints[mountpoint_index].name, file_system_name);
    strcpy(mountpoints[mountpoint_index].mountpoint, mountpoint);
    mountpoints[mountpoint_index].file_operations = file_operations;
    mountpoint_index++;
    return 0;
}

int vfs_get_mountpoint_id(const char *path, vnode_t *vnode) {
    size_t last = 0;
    int lastlen = 0;
    if (strlen(path) == 0) {
        return last;
    }
    for(int i=1; i < MOUNTPOINTS_MAX; i++) {
        int result = strncmp(path, mountpoints[i].mountpoint, strlen(mountpoints[i].mountpoint));
        //pretty_logf(Verbose,"%d\n", i);
        if(mountpoints[i].mountpoint[0] != '\0' &&  result == 0) {
            if(strlen(mountpoints[i].mountpoint) > lastlen) {
                lastlen = strlen(mountpoints[i].mountpoint);
                last = i;
            }
        }
    }
    return last;
}

int vfs_lookup(const char *path, int flags, vnode_t *vnode) {
    int mountpoint_id = vfs_get_mountpoint_id(path, vnode);
    if (mountpoint_id < 0) {
        return -1;
    }
    pretty_logf(Verbose, " --- mountpoint id for file: %d and flags: %d ", mountpoint_id, flags);
    mountpoint_t mountpoint = mountpoints[mountpoint_id];
    pretty_logf(Verbose, " --- mountpoint id for file: %s", mountpoint.mountpoint);
    char *relative_path = vfs_get_relative_path(mountpoint.mountpoint, path);
    pretty_logf(Verbose, " --- relative path is: %s", relative_path);
    // This can be removed
    if (mountpoint.file_operations.open == NULL) {
        return -1;
    }
    
    if (mountpoint.vnode_operations.lookup == NULL) {
        return -1;
    }

    int error_code = mountpoint.vnode_operations.lookup(&relative_path[1], flags, vnode);
    if ( error_code == 0) {
        pretty_log(Verbose, "Setting mountpoint for vnode");
        vnode->vfs_root = &mountpoints[mountpoint_id];
    }
    pretty_logf(Verbose, "File size is: %d", vnode->size);
    // This will be removed, and replaced by the line above
    int driver_fd = mountpoint.file_operations.open(relative_path, flags);
    if (driver_fd < 0) {
        return -1;
    }

    if ( vnode_index > OPENEDFILES_MAX ) {
        return -1;
    }

    vnode->refcount++;
    return 0;
}

char *vfs_get_relative_path (char *root_prefix, char *absolute_path) {
    int root_len = strlen(root_prefix);
    pretty_logf(Verbose, "Removing prefix: %s (len: %d) from absolute path: %s it should be: %s", root_prefix, root_len, absolute_path, &absolute_path[root_len]);
    return &absolute_path[root_len];
}

int vfs_read(vnode_t *vnode, void *buf, int flags, size_t nbytes) {
    pretty_logf(Verbose, "Reading file: %x - flags: %d", vnode->vfs_root, flags);
    if (vnode->vfs_root != NULL) {
        mountpoint_t* mountpoint = vnode->vfs_root;        
        if (vnode->v_data != NULL) {
            ssize_t bytes_read = mountpoint->file_operations.read(vnode, 0, buf, nbytes);
            return bytes_read;
        }
    }
    return 0;
}

int vfs_close (vnode_t *vnode) {    
    pretty_logf(Verbose, "Vnode refcount value: %d", vnode->refcount);
    if (vnode->vfs_root != NULL) {
        mountpoint_t *mountpoint = vnode->vfs_root;
        if (mountpoint->file_operations.close != NULL) {
            //TODO: should pass a vnode to the close operation
            mountpoint->file_operations.close(vnode);
        }
    }
    vnode->refcount--;
    if(vnode->refcount == 0) {
        vnode_clear(vnode);
    }
    return 0;
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
