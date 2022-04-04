#ifndef __KEYBOARD_H_
#define __KEYBOARD_H_

#include <stdint.h>

#define KEYBOARD_ENCODER_PORT   0x60
#define KEYBOARD_STATUS_REGISTER   0x64
#define KEYBOARD_COMMAND_REGISTER   KEYBOARD_SATATUS_REGISTER
#define KEY_RELEASE_MASK    0x80
#define KEYBOARD_ACK_BYTE   0xFA

#define MAX_KEYB_BUFFER_SIZE 0x13

#define SET_RELEASED_STATUS(status)(status = (status|1) )
#define SET_PRESSED_STATUS(status)(status = (status&(~0b00000001)) )

#define BUF_STEP(x) ((x < MAX_KEYB_BUFFER_SIZE -1) ? x+1 : 0  )
typedef enum {

    ESCAPE=0000,
    F1=0001,
    F2=0002,
    F12=0012
    //etc
} key_codes;

typedef struct {
    key_codes code;
    uint8_t modifiers; //Modifiers bit[0]: pressed/released
} key_status;

void init_keyboard();
void handle_keyboard_interrupt();

key_codes translate(uint8_t);
#endif
