#include <logging.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include <ustar.h>

int ustar_open(const char *path, int flags, ...) {
    pretty_logf(Verbose, "called with path: %s and flags: %d", path, flags);
    return 3;
}

int ustar_close(int ustar_fildes) {
    pretty_logf(Verbose, "called with fildes: %d", ustar_fildes);
    return 0;
}

ssize_t ustar_read(int ustar_fildes, char *buf, size_t nbytes) {
    (void)ustar_fildes;
    (void)nbytes;
    strcpy(buf, "Test string");
    return 12;
}

/*ustar_item* ustar_seek(char *filename, ustar_item* tar_root){
    int n_zero_items = 0;
    while (n_zero_items < 2) {
        if (tar_is_zeroed(tar_item)) n_zero_items++;

    }
    return NULL;
}*/

bool tar_is_zeroed(ustar_item *tar_item){
    uint64_t *ustar_conv = (uint64_t *) tar_item;
    short int counter = 0;
    while ( counter < 64) {
        if (ustar_conv[counter] != 0) return false;
        counter++;
    }
    return true;
}
