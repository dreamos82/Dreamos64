#ifndef _SYS_READ_H
#define _SYS_READ_H

#include <unistd.h>

ssize_t sys_read(int fildes, void *buf, size_t nbytes);

ssize_t sys_read_keyboard(void *buffer, size_t nbytes);

#endif
