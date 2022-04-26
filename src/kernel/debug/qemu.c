#include <kernel/qemu.h>
#include <kernel/serial.h>
#include <kernel/io.h>

unsigned int qemu_init_debug(){
    return init_serial(QEMU_LOG_SERIAL_PORT);
}

void qemu_write_char(const char ch){
    while((inportb(QEMU_LOG_SERIAL_PORT + 5) & 0x20) == 0);
    outportb(QEMU_LOG_SERIAL_PORT, ch);
}

void qemu_write_string(const char *string){
    while (*string != '\0'){
        qemu_write_char(*string);
        string++;
    }
}

void debugcon_write_string(const char* message){
    while (*message != '\0'){
        debugcon_write_char(*message);
        message++;
    }
}

void debugcon_write_char(char c){
    outportb(QEMU_DEBUGCON_PORT, c);
}
