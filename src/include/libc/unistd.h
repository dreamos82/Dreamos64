#ifndef __UNISTD_H__
#define __UNISTD_H__

#include <sys/types.h>
#include <stddef.h>

ssize_t read(int fildes, void *buf, size_t nbyte);

#endif
