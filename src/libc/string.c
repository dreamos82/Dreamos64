#include <string.h>
#include <stdint.h>
#ifndef TEST
#include <video.h>
#endif

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
