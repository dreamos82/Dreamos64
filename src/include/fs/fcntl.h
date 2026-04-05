#ifndef _FCNTL_H_
#define _FCNTL_H_

#define O_CREAT 1
#define O_READ 2
#define O_RDWR 4

int open(const char *, int);

// This will be moved in vfs.c file
int vfs_open(const char *path, int flags);

#endif
