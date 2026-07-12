#include <framebuffer.h>
#include <sys_print.h>


void sys_print(char *buffer, size_t nbytes, size_t pos_x, size_t pos_y) {
    if (pos_x == 0 && pos_y == 0) {
        _fb_printStr(buffer, 0x27F549, 0x000000);
        //_fb_printStr("The line above is printed after the read syscall.", 0xD3F527, 0x000000);
    } else {
        _fb_printStrAt(buffer, pos_x, pos_y, 0x27F549, 0x000000);
        //_fb_printStrAt("The line above is printed after the read syscall.", pos_x, pos_y + 1, 0xD3F527, 0x000000)
    }
}
