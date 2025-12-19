#ifndef __USTAR_H_
#define __USTAR_H_

#include <stddef.h>
#include <sys/types.h>

#define HEADER_SIZE 512
#define PADDING_BYTE    0
#define USTAR_VERSION   "00"
#define USTAR_FILENAME_SIZE 100

struct ustar_item {
    char filename[USTAR_FILENAME_SIZE];
    char file_mode[8];
    char    uid[8];
    char    gid[8];
    char file_size[12];
    char version[2];
};

typedef struct ustar_item ustar_item;
int ustar_open(const char *path, int flags, ...);
int ustar_close(int fildes);
ssize_t ustar_read(int fildes, char *buf, size_t nbytes);
int octascii_to_dec(char *number, int size);

#endif
