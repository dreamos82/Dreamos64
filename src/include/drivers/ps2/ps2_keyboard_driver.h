#ifndef __KEYBOARD_DRIVER__
#define __KEYBOARD_DRIVER__

#include <devices.h>

struct driver* ps2_keyboard_driver;

void init_keyboard_device_driver();
void _ps2_keyboard_read(uint8_t *dst, uint8_t len);

/*
 * This structure contains a single operation for the ps2 keyboard.
 * userspace buffer
 */
typedef struct ps2__operation {
    userspace_buffer_t buffer;
    size_t count;
    bool read;
} ps2_operation

//void _ps2_keyboard

#endif
