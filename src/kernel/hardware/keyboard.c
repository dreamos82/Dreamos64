#include <keyboard.h>
#include <framebuffer.h>
#include <stdio.h>
#include <io.h>
#include <ps2.h>
#include <kernel.h>

//extern kernel_status_t kernel_settings;

key_status keyboard_buffer[MAX_KEYB_BUFFER_SIZE];
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
                _fb_printStrAndNumber(" Key released: 0x", scancode, 0, 10, 0x000000, 0xE169CD);
            #endif
            printf("---A key is released: %d -  %x - %x - 0x%x\n", buf_position, scancode, keyboard_buffer[buf_position].code, keyboard_buffer[buf_position].modifiers);
         } else {
            SET_PRESSED_STATUS(keyboard_buffer[buf_position].modifiers);
            #if USE_FRAMEBUFFER == 1
                _fb_printStrAndNumber("  Key pressed: 0x", scancode, 0, 10, 0x000000, 0xE169CD);
            #endif
            printf("---A key is pressed %d -  %x - %x - 0x%x\n", buf_position, scancode, keyboard_buffer[buf_position].code, keyboard_buffer[buf_position].modifiers);
        }
        buf_position = BUF_STEP(buf_position);
    } 
}

key_codes translate(uint8_t scancode) {
    return F12;
}
