#include <logging.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include <ustar.h>
#include <utils.h>

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

ustar_item* ustar_seek(char *filename, ustar_item* tar_root){
    int n_zero_items = 0;
    ustar_item* tar_item = tar_root;
    char *ptr = (char*) tar_root;
    while (n_zero_items < 2) {
        if (ustar_is_zeroed(tar_item)) {
            n_zero_items++;
        } else {
            int filesize = octascii_to_dec(tar_item->file_size, 12);
            int comparison_result_filename = strcmp(filename, tar_item->filename);
            if (comparison_result_filename == 0) {
                return tar_item;
            }
            //ptr = (char *)(ptr + (uint64_t)filesize + (uint64_t)sizeof(struct ustar_item));
            ptr += (((filesize + 511) / 512) + 1) * 512;
            tar_item = (ustar_item *) ptr;
        }
    }
    return NULL;
}

bool ustar_is_zeroed(ustar_item *tar_item){
    uint64_t *ustar_conv = (uint64_t *) tar_item;
    short int counter = 0;
    while ( counter < 64) {
        if (ustar_conv[counter] != 0) return false;
        counter++;
    }
    return true;
}
