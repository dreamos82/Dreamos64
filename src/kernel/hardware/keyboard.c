#include <keyboard.h>
#include <stdio.h>
#include <io.h>

void handle_keyboard_interrupt() {
    
    int scancode = inportb(KEYBOARD_ENCODER_PORT);
    
    if(scancode & KEY_RELEASE_MASK) {
        printf("---A key is released booh... %x\n", scancode);
    } else {
        #if USE_FRAMEBUFFER == 1
            _fb_printStrAndNumber(" Key pressed: 0x", scancode, 0, 9, 0x000000, 0xE169CD);

        #endif
        printf("---A key is pressed hooray... %x\n", scancode);
    } 
}
