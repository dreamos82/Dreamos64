#include <string.h>
#include <stdint.h>
#include <kheap.h>
#ifndef TEST
#include <video.h>
#endif

#define MEMMOVE_SMALL_COPY_SIZE 0x200

char *strcpy(char *dst, const char *src) {
    char* output = dst;
    while (*src != '\0') {
        *output++ = *src++;
    }
    *output = '\0';
    return dst;
}

char* strncpy(char* dest, const char* src, size_t num)
{
    size_t i = 0;
    while (src[i] != 0 && i < num)
    {
        dest[i] = src[i];
        i++;
    }

    //as per spec, dest should be padded with zeroes so it is *num* chars long
    while (i < num)
    {
        dest[i] = 0;
        i++;
    }

    return dest;
}

char* strcat(char* dest, const char* src)
{
    const size_t count = strlen(src);
    const size_t begin = strlen(dest);
    
    for (size_t i = 0; i < count; i++)
        dest[begin + i] = src[i];
    dest[begin + count] = 0;

    return dest;
}

char* strncat(char* dest, const char* src, size_t count)
{
    const size_t source_len = strlen(src);
    const size_t begin = strlen(dest);
    if (source_len < count)
        count = source_len;
    
    for (size_t i = 0; i < count; i++)
        dest[begin + i] = src[i];
    dest[begin + count] = 0;

    return dest;
}

size_t strlen(const char* str)
{
    size_t len = 0;
    while (str[len] != 0)
        len++;
    return len;
}

int strcmp(const char *s1, const char *s2){
    while(*s1 && (*s1 == *s2)){
        s1++;
        s2++;   
    }

    int ret = *(const unsigned char*)s1 - *(const unsigned char*)s2;
    
    if(ret < 0) {
        return -1;
    } else if(ret > 0) {
        return 1;
    }
    return 0;
    //    return *(const unsigned char*)s1 - *(const unsigned char*)s2;
}

int strncmp(const char *s1, const char *s2, size_t n){
    if(!n) return 0;

    while(--n && *s1 && (*s1 == *s2)){
        s1++;
        s2++;   
    }
    //return *(const unsigned char*)s1 - *(const unsigned char*)s2;
    int ret = *(const unsigned char*)s1 - *(const unsigned char*)s2;
    
    if(ret < 0) {
        return -1;
    } else if(ret > 0) {
        return 1;
    }
    return 0;

}

char* strchr(char* src, int ch)
{
    char* scan = src;
    while (*scan != ch && *scan != 0)
        scan++;
    
    //searching for the null terminator is valid, so the spec says.
    //so we need to return NULL if they weren't searching for the end of the string.
    if (*scan == 0 && ch != 0)
        return NULL;
    return scan;
}

char* strrchr(char* src, int ch)
{
    const size_t length = strlen(src);
    char* scan = &src[length];

    while ((uintptr_t)scan >= (uintptr_t)src)
    {
        if (*scan == ch)
            return scan;
        scan--;
    }

    return NULL;
}

void* memchr(void* src, int ch, size_t count)
{
    unsigned char* scan = src;
    for (size_t i = 0; i < count; i++)
    {
        if (scan[i] == ch)
            return &scan[i];
    }

    return NULL;
}

int memcmp(const void* a, const void* b, size_t limit)
{
    const unsigned char* ap = a;
    const unsigned char* bp = b;

    for (size_t i = 0; i < limit; i++)
    {
        if (ap[i] < bp[i])
            return -1;
        if (ap[i] > bp[i])
            return 1;
    }

    return 0;
}

void* memset(void* dest, int ch, size_t count)
{
    uint8_t* dst = dest;
    for (size_t i = 0; i < count; i++)
        dst[i] = ch;

    return dest;
}

void* memcpy(void* dest, void* src, size_t count)
{
    //these casts are not necessary if using gcc extensions to allow pointer arithmatic
    //on void pointers, but this is more portable and will be optimized away.
    const uint8_t* source = src;
    uint8_t* destination = dest;

    for (size_t i = 0; i < count; i++)
        destination[i] = source[i];

    return dest;
}

void* memmove(void* dest, void* src, size_t count)
{
    //memmove must act like it supports an internmediate buffer (i.e. if we src and dest overlap, it must work as expected).
    //first lets check if the buffers even overlap, if they dont it's just memcpy.
    //if they do, we'll decide whether we want to allocate the intermediate buffer
    //on the stack (for small copies, its much faster), or the heap.
    const uintptr_t dest_top = (uintptr_t)dest + count;
    const uintptr_t src_top = (uintptr_t)src + count;
    if (dest_top < (uintptr_t)src || src_top < (uintptr_t)dest)
        return memcpy(dest, src, count);

    uint8_t* buff;
    if (count <= MEMMOVE_SMALL_COPY_SIZE)
        buff = __builtin_alloca(count);
    else
        buff = kmalloc(count);
    
    //see memcopy for why these variables are used.
    const uint8_t* source = src;
    uint8_t* destination = dest;
    for (size_t i = 0; i < count; i++)
        buff[i] = source[i];
    for (size_t i = 0; i < count; i++)
        destination[i] = source[i];

    if (count <= MEMMOVE_SMALL_COPY_SIZE)
        kfree(buff);

    return dest;
}

void test_strcmp(){
    int result = strcmp("hello", "world");
    if(result != 0) {
        _printStr("hello != world\n");
    }
    result = strcmp("hello", "hello");
    if(result == 0){
        _printStr("hello == hello\n");
    }
    result = strcmp("world", "hello");
    if(result != 0){
        _printStr("world != hello\n");
    }
    result = strcmp("", "");
    if (result == 0){
        _printStr("empty strings are equals\n");
    }
}
