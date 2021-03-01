#ifndef _QEMU_H
#define _QEMU_H

#define QEMU_LOG_SERIAL_PORT    0x3f8

unsigned int qemu_init_debug();
void qemu_write_string(char *message);
void qemu_write_char(char);

#endif
