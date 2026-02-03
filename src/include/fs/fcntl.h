#ifndef _FCNTL_H_
#define _FCNTL_H_

int open(const char *, int);

// This will be moved in vfs.c file
int vfs_open(const char *path, int flags);

#endif
