#include <keyboard.h>
#include <framebuffer.h>
#include <stdio.h>
#include <io.h>
#include <ps2.h>
#include <kernel.h>

extern kernel_status_t kernel_settings;

void init_keyboard(){
    //Let's do a keyboard read just to make sure it's empty
    inportb(KEYBOARD_ENCODER_PORT);
    // The following two bytes will read the scancode set used by the keyboard
    outportb(KEYBOARD_ENCODER_PORT, 0xF0);
    outportb(KEYBOARD_ENCODER_PORT, 0x00);
    uint8_t status_read = inportb(KEYBOARD_ENCODER_PORT);
    printf("First byte read: 0x%x - ", status_read);
    if(status_read == KEYBOARD_ACK_BYTE) {
        status_read = inportb(KEYBOARD_ENCODER_PORT);    
        printf("Found scancode set: 0x%x\n", status_read);
        kernel_settings.keyboard.scancode_set = status_read;
    }
    outportb(0x64, 0x20);
    
    while((inportb(0x64) & 2) != 0) {
        status_read = inportb(0x64);
        printf("Not ready yet... %x\n", status_read);
    }
    uint8_t configuration_byte = inportb(0x60);
    printf("Configuration byte: %x\n", configuration_byte);
    if((configuration_byte & (1 << 6)) != 0) {
        printf("Translation enabled\n");
        kernel_settings.keyboard.translation_enabled = true;
    }
}

void handle_keyboard_interrupt() {
    
    int scancode = inportb(KEYBOARD_ENCODER_PORT);    
    
    if(scancode & KEY_RELEASE_MASK) {
        #if USE_FRAMEBUFFER == 1
            _fb_printStrAndNumber(" Key released: 0x", scancode, 0, 10, 0x000000, 0xE169CD);

        #endif

        printf("---A key is released booh... %x\n", scancode);
    } else {
        #if USE_FRAMEBUFFER == 1
            _fb_printStrAndNumber("  Key pressed: 0x", scancode, 0, 10, 0x000000, 0xE169CD);

        #endif
        printf("---A key is pressed hooray... %x\n", scancode);
    } 
}
