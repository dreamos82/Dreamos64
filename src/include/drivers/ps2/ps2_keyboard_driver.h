#ifndef __KEYBOARD_DRIVER__
#define __KEYBOARD_DRIVER__

#include <devices.h>

extern struct driver* ps2_keyboard_driver;

extern pending_operation_t *_ps2_op_head;
extern pending_operation_t *_ps2_op_tail;

/*
 * This structure contains a single operation for the ps2 keyboard.
 * userspace buffer is a special structure that contains the mapping of the userspace buffer in kernel space.
 * Once we wnat to issue a ps2 operation we create a new isntance of thist structure.
 */
typedef struct ps2_operation {
    userspace_buffer_t buffer;
    size_t count;
    bool read;
    struct ps2_operation *next;
} ps2_operation;

void init_keyboard_device_driver();
void _ps2_keyboard_read(uint8_t *dst, uint8_t len);
void _ps2_keyboard_del_operation(pending_operation_t *op_to_delete);
void _ps2_keyboard_add_operation(pending_operation_t *new_op);

#endif
