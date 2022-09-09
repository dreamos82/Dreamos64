#ifndef _VFS_H
#define _VFS_H

struct {
    char name[32];  // The filesystem name    
} file_system_node;



typedef struct mountpoint_t mountpoint_t;

void vfs_init();
#endif
