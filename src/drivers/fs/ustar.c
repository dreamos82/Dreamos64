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

int ustar_close(vnode_t *vnode) {
    //The code below is only for debug purposes, it will be removed in the future.
    ustar_item *file_to_close= (ustar_item *)vnode->v_data;
    pretty_logf(Verbose, "called with file name: %s 0 - Nothing to do for Close", file_to_close->filename);
    return 0;
}

ssize_t ustar_read(vnode_t *vnode, int ustar_fildes, char *buf, size_t nbytes) {
    (void)ustar_fildes;
    (void)nbytes;
    //strcpy(buf, "Test string");
    if (vnode->v_data != NULL) {
        ustar_item *item_to_read = (ustar_item*) vnode->v_data;
        char *read_buffer = ustar_get_file_start(item_to_read);
        if (read_buffer != NULL) {
            int i = 0;
            for (i=0; i < nbytes; i++) {                
                buf[i] = read_buffer[i];
                if (read_buffer[i] == '\0') {
                    break;
                }
            }
            //pretty_logf(Verbose, "Read string: %s", buf);
            return i;
        }
    }
    return 0;
}


int ustar_lookup(const char *path, int flags, vnode_t *vnode){
    pretty_logf(Verbose, "path: %s - root_item: 0x%x", path, ustar_root_fs.root_item);
    //Rootfs should be passed as parameter of ustar_lookup, since there could be ,ultiple file systems using the same driver.
    ustar_item *item_to_return = ustar_seek(path, ustar_root_fs.root_item);
    //ustar_find(path, ustar_root_fs.root_item, (ustar_item **) vnode->v_data);
    if (item_to_return == NULL) {
        //No file with that name has been found
        return -1;
    }
    pretty_logf(Verbose, "Type: %d", item_to_return->typeflag);
    if ( item_to_return->typeflag == '0') {
        vnode->v_type = ustar_get_type(item_to_return->typeflag);
        if (vnode->v_type == V_REGULAR_FILE) {
            pretty_log(Verbose, "Is a regular file");
        }
    }
    
    vnode->v_data = (void *) item_to_return;
    // I have to convert the size from octal ascii to decimal.
    vnode->size = octascii_to_dec(item_to_return->file_size, USTAR_FILESIZE_SIZE);
    return 0;
}

vnode_types ustar_get_type(char type){
    switch(type){
        case '0':
            return V_REGULAR_FILE;
        case '5':
            return V_DIR;
        default:
            return V_BAD;
    }
    
    return V_BAD;
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

ustar_item* ustar_seek(char *filename, ustar_item* tar_root) {
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

char* ustar_get_file_start(ustar_item *file_item) {
    if (file_item != NULL) {
        char *ptr = (char *) file_item;                
        return ptr+512;
        //return (file_item + ptr);
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
