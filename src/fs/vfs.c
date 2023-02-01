#include <vfs.h>
#include <ustar.h>
#include <logging.h>
#include <string.h>

mountpoint_t mountpoints[MOUNTPOINTS_MAX];
unsigned int vfs_fd_index;


void vfs_init() {
    logline(Verbose, "(vfs_init) Initializiing VFS layer");
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
}

int get_mountpoint_id(char *path) {
    int last = 0;
    int lastlen = 0;
    if (strlen(path) == 0) {
        return last;
    }
    for(int i=1; i < MOUNTPOINTS_MAX; i++) {
        int result = strncmp(path, mountpoints[i].mountpoint, strlen(mountpoints[i].mountpoint));
        //loglinef(Verbose,"%d\n", i);
        if(mountpoints[i].mountpoint[0] != '\0' &&  result == 0) {
            if(strlen(mountpoints[i].mountpoint) > lastlen) {
                lastlen = strlen(mountpoints[i].mountpoint);
                last = i;
            }
        }
    }
    return last;
}

char *get_relative_path (char *root_prefix, char *absolute_path) {
    int root_len = strlen(root_prefix);
    loglinef(Verbose, "(get_relative_path) Removing prefix: %s (len: %d) from absolute path: %s it should be: %s", root_prefix, root_len, absolute_path, &absolute_path[root_len]);
    return &absolute_path[root_len];
}
