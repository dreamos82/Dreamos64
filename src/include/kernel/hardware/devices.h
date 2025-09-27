#ifndef _DEVICES_H
#define _DEVICES_H

#include <stdint.h>
#include <unistd.h>
#include <vmm.h>

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

typedef struct userspace_buffer_t {
    // We need to pass the VMM info in order to be able to get the physical address and then the hhdm mapping of the buffer
    // since we don't want to make a paging switch for every character read.
    VmmInfo info;
    uintptr_t buffer_base; /**< this is the base address received relative to the task */
    uintptr_t buffer_virtual; /**< this the virtual address in kernel space of the buffer */
    size_t length;
} userspace_buffer_t;

typedef struct pending_operation_t {
    userspace_buffer_t *buffer;
    size_t nbytes;
    // TODO: Add a semaphore
    bool read;
    struct pending_operation_t *next;
} pending_operation_t;

typedef struct driver driver_item_t;
//typedef unsigned char _device_descriptor;

typedef struct {
    driver_item_t drivers;
    unsigned char device_id;
    void (*init)(/* init args */);
    void (*shutdown)();
    void (*read)(uint8_t *dst, uint8_t len);
    void (*write)();
    void (*ioctl)();
} device_t;

extern unsigned char _device_descriptor;

void _device_add_driver(device_t device);
void init_known_drivers();

#endif
