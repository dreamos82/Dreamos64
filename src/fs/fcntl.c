#include <fcntl.h>
#include <logging.h>
#include <string.h>
#include <vfs.h>

vfs_file_descriptor_t vfs_opened_files[OPENEDFILES_MAX];


int open(const char *path, int flags) {
    return vfs_open(path, flags);
}

int vfs_open(const char *path, int flags) {
    pretty_logf(Verbose, "Try to open file: %s", path);
    return vfs_lookup(path,flags, NULL);
}
