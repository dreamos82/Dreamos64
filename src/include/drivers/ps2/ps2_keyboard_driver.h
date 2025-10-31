#ifndef __KEYBOARD_DRIVER__
#define __KEYBOARD_DRIVER__

#include <devices.h>

extern struct driver* ps2_keyboard_driver;

extern pending_operation_t *_ps2_op_head;
extern pending_operation_t *_ps2_op_tail;

void _ps2_keyboard_driver_init();

void init_keyboard_device_driver();
void _ps2_keyboard_read(uint8_t *dst, uint8_t len);
void _ps2_keyboard_del_operation(pending_operation_t *op_to_delete);
void _ps2_keyboard_add_operation(pending_operation_t *new_op);
pending_operation_t* _os2_get_next_operation();

#endif
