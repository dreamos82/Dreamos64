#ifndef _QEMU_H
#define _QEMU_H

#define QEMU_LOG_SERIAL_PORT    0x3f8
#define QEMU_DEBUGCON_PORT      0xe9

unsigned int qemu_init_debug();
void qemu_write_string(const char *message);
void qemu_write_char(const char);
void debugcon_write_string(const char* message);
void debugcon_write_char(const char c);

#endif
