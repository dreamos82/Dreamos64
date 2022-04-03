#ifndef __KEYBOARD_H_
#define __KEYBOARD_H_

#define KEYBOARD_ENCODER_PORT   0x60
#define KEYBOARD_STATUS_REGISTER   0x64
#define KEYBOARD_COMMAND_REGISTER   KEYBOARD_SATATUS_REGISTER
#define KEY_RELEASE_MASK    0x80
#define KEYBOARD_ACK_BYTE   0xFA

void init_keyboard();
void handle_keyboard_interrupt();

#endif
