#ifndef _STRING_H
#define _STRING_H

#include <stdint.h>
#include <stddef.h>

int strncmp(const char*, const char*, size_t);
int strcmp(const char *, const char *);
char *strcpy(char *, const char *);
void test_strcmp();
#endif

