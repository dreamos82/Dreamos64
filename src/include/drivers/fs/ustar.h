#ifndef __USTAR_H_
#define __USTAR_H_

#include <stddef.h>
#include <sys/types.h>

#define HEADER_SIZE 512
#define PADDING_BYTE    0
#define USTAR_VERSION   "00"
#define USTAR_FILENAME_SIZE 100
#define USTAR_OCTAL_FIELD_SIZE 12
#define USTAR_MTIME_SIZE USTAR_OCTAL_FIELD_SIZE

struct ustar_item {
    char filename[USTAR_FILENAME_SIZE];
    char file_mode[8];
    char    uid[8];
    char    gid[8];
    char file_size[12];
    char mtime[12];
    char chksum[8];
    char typeflag;
    char linkname[100];
    char magic[6];
    char version[2];
    char owner[32];
    char group[32];
    char devicemajor[8];
    char deviceminor[8];
    char prefix[155];
};

typedef struct ustar_item ustar_item;

void ustar_driver_init();

int ustar_open(const char *path, int flags, ...);
int ustar_close(int fildes);
ssize_t ustar_read(int fildes, char *buf, size_t nbytes);
bool ustar_is_zeroed(ustar_item *tar_item);
ustar_item* ustar_seek(char *filename, ustar_item* tar_root);
ssize_t ustar_find(char *filename, ustar_item* tar_root, ustar_item** tar_out);

#endif
