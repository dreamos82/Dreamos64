#ifndef __KEYBOARD_H_
#define __KEYBOARD_H_

#define KEYBOARD_ENCODER_PORT   0x60
#define KEY_RELEASE_MASK    0x80

void init_keyboard();
void handle_keyboard_interrupt();

#endif
