#include <logging.h>
#include <string.h>
#include <ustar.h>
#include <vfs.h>

mountpoint_t mountpoints[MOUNTPOINTS_MAX];
unsigned int vfs_fd_index;

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
    strcpy(mountpoints[3].mountpoint, "/home");
    mountpoints[3].file_operations.open = ustar_open;
    mountpoints[3].file_operations.close = ustar_close;
    mountpoints[3].file_operations.read = ustar_read;
    vfs_fd_index = 3;
}

int vfs_register(char *file_system_name, char *mountpoint, fs_file_operations_t operations){
    if ( vfs_fd_index >MOUNTPOINTS_MAX )
        return -1;
    strcpy(mountpoints[vfs_fd_index].name, file_system_name);
    strcpy(mountpoints[vfs_fd_index].mountpoint, mountpoint);
    return 0;
}

int vfs_get_mountpoint_id(const char *path) {
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

char *vfs_get_relative_path (char *root_prefix, char *absolute_path) {
    int root_len = strlen(root_prefix);
    pretty_logf(Verbose, "Removing prefix: %s (len: %d) from absolute path: %s it should be: %s", root_prefix, root_len, absolute_path, &absolute_path[root_len]);
    return &absolute_path[root_len];
}
