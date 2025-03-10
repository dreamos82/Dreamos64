#ifndef _DEVICES_H
#define _DEVICES_H

typedef struct {
    enum {
        fd_invalid_descriptor,
        fd_vnode_descriptor,
        fd_device_descriptor
    } type;
    void *pointer;
} file_descriptor_t;

struct driver{
    struct driver *next;
};

typedef struct driver driver_item_t;

typedef struct {
    driver_item_t drivers;
    unsigned char device_id;
    void (*init)(/* init args */);
    void (*shutdown)();
    void (*read)();
    void (*write)();
    void (*ioctl)();
} device_t;

void _device_add_driver(device_t device);
void init_known_drivers();

#endif
