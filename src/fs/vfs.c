#include <logging.h>
#include <string.h>
#include <ustar.h>
#include <vfs.h>

mountpoint_t mountpoints[MOUNTPOINTS_MAX];
unsigned int vfs_fd_index;
unsigned int vnode_index;

mountpoint_t *mountpoints_list;


void vfs_init() {
    pretty_log(Verbose, "Initializiing VFS layer");
    for (int i=0; i < MOUNTPOINTS_MAX; i++) {
        strcpy(mountpoints[i].name, "");
        strcpy(mountpoints[i].mountpoint, "");
        mountpoints[i].file_operations.open = NULL;
        mountpoints[i].file_operations.close = NULL;
    }

    vfs_fd_index=0;

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
    vfs_fd_index = 3;
    vnode_index = 0;
}

int vfs_register(char *file_system_name, char *mountpoint, fs_file_operations_t operations){
    if ( vfs_fd_index >MOUNTPOINTS_MAX )
        return -1;
    //This shouldn't use vfs_fd_index.
    strcpy(mountpoints[vfs_fd_index].name, file_system_name);
    strcpy(mountpoints[vfs_fd_index].mountpoint, mountpoint);
    //vfs_fd_index++;
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

    int driver_fd = mountpoint.file_operations.open(relative_path, flags);
    if (driver_fd < 0) {
        return -1;
    }

    if ( vfs_fd_index > OPENEDFILES_MAX ) {
        return -1;
    }

    vnode->refcount++;

    vfs_opened_files[vfs_fd_index].fs_specific_id = driver_fd;
    vfs_opened_files[vfs_fd_index].mountpoint_id = mountpoint_id;
    vfs_opened_files[vfs_fd_index].buffer_read_pos = 0;
    strcpy(vfs_opened_files[vfs_fd_index].filename, path);
    return vfs_fd_index++;
}

char *vfs_get_relative_path (char *root_prefix, char *absolute_path) {
    int root_len = strlen(root_prefix);
    pretty_logf(Verbose, "Removing prefix: %s (len: %d) from absolute path: %s it should be: %s", root_prefix, root_len, absolute_path, &absolute_path[root_len]);
    return &absolute_path[root_len];
}
