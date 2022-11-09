#include <vfs.h>
#include <logging.h>
#include <string.h>

mountpoint_t mountpoints[MOUNTPOINTS_MAX];

void vfs_init() {
    logline(Verbose, "Initializiing VFS layer");
    for (int i=0; i < MOUNTPOINTS_MAX; i++) {
        strcpy(mountpoints[i].name, "");
        strcpy(mountpoints[i].mountpoint, "");
    }

    // The first item will always be the root!
    strcpy(mountpoints[0].name, "ArrayFS");
    strcpy(mountpoints[0].mountpoint, "/");
    
    // Adding some fake fs
    strcpy(mountpoints[1].name, "ArrayFS");
    strcpy(mountpoints[1].mountpoint, "/home");
    // Adding some fake fs
    strcpy(mountpoints[2].name, "ArrayFS");
    strcpy(mountpoints[2].mountpoint, "/usr");

}

int get_mountpoint_id(char *path) {
    int last = 0;
    for(int i=1; i < MOUNTPOINTS_MAX; i++) {
        if(mountpoints[i].mountpoint != "" && strncmp(path, mountpoints[i].mountpoint, strlen(mountpoints[i].mountpoint)) == 0) {
            loglinef(Verbose, "Found  %s", mountpoints[i].mountpoint);
            last = i;
        }
    }
    return last;
}
