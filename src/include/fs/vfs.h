#ifndef _VFS_H
#define _VFS_H

#define MOUNTPOINTS_MAX 5
#define FILESYSTEM_NAME_LEN 32
#define MAX_MOUNTPOINT_LEN  64

typedef struct {
    char name[FILESYSTEM_NAME_LEN];  // The filesystem name 

    char mountpoint[MAX_MOUNTPOINT_LEN];   
} mountpoint_t;


extern mountpoint_t mountpoints[MOUNTPOINTS_MAX];
void vfs_init();
#endif
