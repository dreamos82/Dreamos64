#ifndef _VFS_H
#define _VFS_H

#include <stddef.h>
#include <sys/types.h>
#include <vnode.h>

#define MOUNTPOINTS_MAX 5
#define OPENEDFILES_MAX 5
#define FILESYSTEM_NAME_LEN 32
#define MAX_MOUNTPOINT_LEN  64
#define MAX_FILENAME_LEN 32

// Probably the names of the functions here will be changed soon along with the args.
struct fs_file_operations_t{     
    int (*open)(const char *, int, ... );
    int (*close)(int);
    ssize_t (*read)(vnode_t *, int, char*, size_t);
    ssize_t (*write)(int,const void*, size_t);
};

typedef struct fs_file_operations_t fs_file_operations_t;

typedef struct vnode_ops_t {
    int (*lookup)(const char *, int, vnode_t *);
} vnode_ops_t;

typedef struct mountpoint_t {
    char name[FILESYSTEM_NAME_LEN];  // The filesystem name 

    char mountpoint[MAX_MOUNTPOINT_LEN];
    vnode_ops_t vnode_operations;
    
    fs_file_operations_t file_operations;

} mountpoint_t;


extern mountpoint_t mountpoints[MOUNTPOINTS_MAX];

extern unsigned int vnode_index;
extern unsigned int mountpoint_index;

void vfs_init();
int vfs_get_mountpoint_id(const char *path, vnode_t *vnode);
int vfs_register(char *file_system_name, char *mountpoint, fs_file_operations_t file_operations);
int vfs_lookup(const char *path, int flags, vnode_t *vnode);
int vfs_read(vnode_t *vnode, void *buffer, int flags, size_t nbytes);
int mount_fs(char *mountpoint, char* name, fs_file_operations_t file_operations);
int vfs_close (vnode_t *vnode);
char *vfs_get_relative_path (char *root_prefix, char *absolute_path);
#endif
