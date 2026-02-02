#ifndef _VFS_H
#define _VFS_H

#include <stddef.h>
#include <sys/types.h>

#define MOUNTPOINTS_MAX 5
#define OPENEDFILES_MAX 5
#define FILESYSTEM_NAME_LEN 32
#define MAX_MOUNTPOINT_LEN  64
#define MAX_FILENAME_LEN 32

// Probably the names of the functions here will be changed soon along with the args.
struct fs_file_operations_t{ 
	int (*open)(const char *, int, ... );
	int (*close)(int);
	ssize_t (*read)(int, char*, size_t);
	ssize_t (*write)(int,const void*, size_t);
};

struct vfs_file_descriptor_t {
    
    int fs_specific_id;
    int mountpoint_id;
    char filename[MAX_FILENAME_LEN];

    int buffer_read_pos;
    char *buffer;

};

typedef struct vfs_file_descriptor_t vfs_file_descriptor_t;
typedef struct fs_file_operations_t fs_file_operations_t;

typedef struct {
    char name[FILESYSTEM_NAME_LEN];  // The filesystem name 

    char mountpoint[MAX_MOUNTPOINT_LEN];
    
    fs_file_operations_t file_operations;

} mountpoint_t;


extern mountpoint_t mountpoints[MOUNTPOINTS_MAX];
extern vfs_file_descriptor_t vfs_opened_files[OPENEDFILES_MAX];

extern unsigned int vfs_fd_index;

void vfs_init();
int vfs_get_mountpoint_id(const char *path);
int vfs_lookup(const char *path, int flags);
int mount_fs(char *mountpoint, char* name, fs_file_operations_t file_operations);
char *vfs_get_relative_path (char *root_prefix, char *absolute_path);
#endif
