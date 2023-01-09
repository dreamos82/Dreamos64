#ifndef __USTAR_H_
#define __USTAR_H_

#define HEADER_SIZE 512
#define PADDING_BYTE    0
#define USTAR_VERSION   "00"
#define USTAR_FILENAME_SIZE 100

struct {
    char filename[USTAR_FILENAME_SIZE];
    char file_mode[8];
    char    uid[8];
    char    gid[8];
    char version[2];
} ustar_item;
int ustar_open(char *path, int flags);
#endif
