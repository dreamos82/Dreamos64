#include <keyboard.h>
#include <io.h>

void handle_keyboard_interrupt() {
    
    int scancode = inportb(KEYBOARD_ENCODER_PORT);
    
    if(scancode & KEY_RELEASE_MASK) {
        printf("---A key is released booh... %x\n", scancode);
    } else {
        printf("---A key is pressed hooray... %x\n", scancode);
    } 
}
