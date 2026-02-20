#include <logging.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include <ustar.h>
#include <utils.h>

//This is a temporary fs for early development.
ustar_mount ustar_root_fs;


void ustar_driver_init(void *ustar_root_address) {
    ustar_root_fs.root_item = (ustar_item *) ustar_root_address;
}

int ustar_open(const char *path, int flags, ...) {
    pretty_logf(Verbose, "called with path: %s and flags: %d", path, flags);
    //ustar_find()
    return 3;
}

int ustar_close(int ustar_fildes) {
    pretty_logf(Verbose, "called with fildes: %d 0 - Nothing to do for Close", ustar_fildes);
    return 0;
}

ssize_t ustar_read(int ustar_fildes, char *buf, size_t nbytes) {
    (void)ustar_fildes;
    (void)nbytes;
    strcpy(buf, "Test string");
    return 12;
}

int ustar_lookup(const char *path, int flags, vnode_t *vnode){
    pretty_logf(Verbose, "path: %s - root_item: 0x%x", path, ustar_root_fs.root_item);
    //Rootfs should be passed as parameter of ustar_lookup, since there could be ,ultiple file systems using the same driver.
    ustar_item *item_to_return = ustar_seek(path, ustar_root_fs.root_item);
    //ustar_find(path, ustar_root_fs.root_item, (ustar_item **) vnode->v_data);
    if (item_to_return == NULL) {
        pretty_log(Verbose, "Item not found");
    } else {
        pretty_log(Verbose, "Item found");
    }
    pretty_logf(Verbose, "item_to_return address: 0x%x", item_to_return)
    vnode->v_data = (void *) item_to_return;
    return 0;
}

ssize_t ustar_find(char *filename, ustar_item* tar_root, ustar_item** tar_out) {
    uint32_t counter = 0;
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
                pretty_logf(Verbose, "File found: %s", tar_item->filename);
                *tar_out = tar_item;
                return counter;
            }
            //ptr = (char *)(ptr + (uint64_t)filesize + (uint64_t)sizeof(struct ustar_item));
            ptr += (((filesize + 511) / 512) + 1) * 512;
            n_zero_items = 0;
            tar_item = (ustar_item *) ptr;
        }
        counter++;
    }
    *tar_out = NULL;
    return -1;
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
