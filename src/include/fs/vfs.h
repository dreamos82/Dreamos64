#ifndef _VFS_H
#define _VFS_H

#include <stddef.h>
#include <sys/types.h>

#define MOUNTPOINTS_MAX 5
#define FILESYSTEM_NAME_LEN 32
#define MAX_MOUNTPOINT_LEN  64

struct fs_file_operations_t{ 
	int (*open)(const char *, int, ... );
	int (*close)(int);
	ssize_t (*read)(int, char*, size_t);
	ssize_t (*write)(int,const void*, size_t);
};

typedef struct fs_file_operations_t fs_file_operations_t;

typedef struct {
    char name[FILESYSTEM_NAME_LEN];  // The filesystem name 

    char mountpoint[MAX_MOUNTPOINT_LEN];
    
    fs_file_operations_t file_operations;

} mountpoint_t;


extern mountpoint_t mountpoints[MOUNTPOINTS_MAX];

void vfs_init();
int get_mountpoint_id(char *path);
#endif
