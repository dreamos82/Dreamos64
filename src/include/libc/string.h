#ifndef _STRING_H
#define _STRING_H

#include <stdint.h>
#include <stddef.h>


char* strcpy(char *dest, const char *src);
char* strncpy(char* dest, const char* src, size_t num);
char* strcat(char* dest, const char* src);
char* strncat(char* dest, const char* src, size_t count);

size_t strlen(const char* str);
int strcmp(const char * a, const char * b);
int strncmp(const char* a, const char* b, size_t limit);
char* strchr(char* src, int ch);
char* strrchr(char* src, int ch);

void* memchr(void* src, int ch, size_t count);
int memcmp(const void* a, const void* b, size_t limit);
void* memset(void* dest, int ch, size_t count);
void* memcpy(void* dest, void* src, size_t count);
void* memmove(void* dest, void* src, size_t count);

void test_strcmp();
#endif
