#include <keyboard.h>
#include <framebuffer.h>
#include <stdio.h>
#include <io.h>
#include <ps2.h>
#include <kernel.h>

//extern kernel_status_t kernel_settings;

key_status keyboard_buffer[MAX_KEYB_BUFFER_SIZE];
key_codes scancode_mappings[] = {
0, ESCAPE, D1, D2, D3, D4, D5, D6, D7, D8, D9, D0, MINUS, EQUALS, BACKSPACE,
TAB, Q, W, E, R, T, Y, U, I, O, P, SQBRACKET_OPEN, SQBRACKET_CLOSE, ENTER,
LEFT_CTRL, A, S, D, F, G, H, J, K, L, SEMICOLON, SINGLE_QUOTE, BACK_TICK,
LEFT_SHIFT, SLASH, Z, X, C, V, B, N, M, COMMA, DOT, BACKSLASH, RIGHT_SHIFT,
KEYPAD_STAR, LEFT_ALT, SPACE, CAPS_LOCK,
F1, F2, F3, F4, F5, F6, F7, F8, F9, F10, 
F11, F12, NUM_LOCK, SCROLL_LOCK,
KEYPAD_D7, KEYPAD_D8, KEYPAD_D9, KEYPAD_MINUS, KEYPAD_D4, KEYPAD_D5, KEYPAD_D6, KEYPAD_PLUS, KEYPAD_D1, KEYPAD_D2, KEYPAD_D3, KEYPAD_D0, KEYPAD_DOT,
};

size_t buf_position;

void init_keyboard(){
    //Let's do a keyboard read just to make sure it's empty
    inportb(KEYBOARD_ENCODER_PORT);
    // The following two bytes will read the scancode set used by the keyboard
    outportb(KEYBOARD_ENCODER_PORT, 0xF0);
    outportb(KEYBOARD_ENCODER_PORT, 0x00);
    uint8_t status_read = inportb(KEYBOARD_ENCODER_PORT);
    if(status_read == KEYBOARD_ACK_BYTE) {
        status_read = inportb(KEYBOARD_ENCODER_PORT);    
        printf("Found scancode set: 0x%x\n", status_read);
        kernel_settings.keyboard.scancode_set = status_read;
    } else {
        printf("Unable to read from keyboard, ... \n");
    }
    outportb(0x64, PS2_READ_CONFIGURATION_COMMAND);
    status_read = inportb(PS2_STATUS_REGISTER);
    while((status_read & 2) != 0) {
        printf("Not ready yet... %x\n", status_read);
        status_read = inportb(PS2_STATUS_REGISTER);
    }
    uint8_t configuration_byte = inportb(PS2_DATA_REGISTER);    
    if((configuration_byte & (1 << 6)) != 0) {
        printf("Translation enabled\n");
        kernel_settings.keyboard.translation_enabled = true;
    } else {
        kernel_settings.keyboard.translation_enabled = false;        
    }
    buf_position = 0;
}

void handle_keyboard_interrupt() {
    
    int scancode = inportb(KEYBOARD_ENCODER_PORT);
  
    if(kernel_settings.keyboard.translation_enabled == true || kernel_settings.keyboard.scancode_set) {
        keyboard_buffer[buf_position].code = translate(scancode);
        if(scancode & KEY_RELEASE_MASK) {
            SET_RELEASED_STATUS(keyboard_buffer[buf_position].modifiers);
            #if USE_FRAMEBUFFER == 1
                _fb_printStrAndNumber(" Key released: 0x", translate(scancode), 0, 10, 0x000000, 0xE169CD);
            #endif
            printf("---A key is released: %d - SC:  %x - Code:  %x - Mod: 0x%x\n", buf_position, scancode, keyboard_buffer[buf_position].code, keyboard_buffer[buf_position].modifiers);
         } else {
            SET_PRESSED_STATUS(keyboard_buffer[buf_position].modifiers);
            #if USE_FRAMEBUFFER == 1
                _fb_printStrAndNumber("  Key pressed: 0x", translate(scancode), 0, 10, 0x000000, 0xE169CD);
            #endif
            printf("---A key is pressed %d -  SC: %x - Code: %x - Mod: 0x%x\n", buf_position, scancode, keyboard_buffer[buf_position].code, keyboard_buffer[buf_position].modifiers);
        }
        buf_position = BUF_STEP(buf_position);
    } 
}

key_codes translate(uint8_t scancode) {
    if(scancode == 0xE0) {
        printf("Extended byte\n");
    }
    if(scancode < 0x47) {
        printf("Translated: %x\n", scancode_mappings[scancode]);
        return scancode_mappings[scancode];
    }
    return scancode_mappings[0];
}
