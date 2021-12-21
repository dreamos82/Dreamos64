#ifndef _STDIO_H
#define _STDIO_H

#include <stdarg.h>

int printf(const char *fmt, ...);
int vsprintf(char *buffer, const char *fmt, va_list args);
static int skip_atoi(const char**);
#endif
