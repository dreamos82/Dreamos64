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

    strcpy(mountpoints[0].name, "ArrayFS");
    strcpy(mountpoints[0].mountpoint, "/");
}
