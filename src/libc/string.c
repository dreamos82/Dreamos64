#include <string.h>

/*int strncmp(const char *s1, const char *s2, size_t n){
    return 0;
}*/

int strcmp(const char *s1, const char *s2){
    int ret_value = 0;
    while(*s1 && (*s1 == *s2)){
        s1++;
        s2++;
    }
    return *(const unsigned char*)s1 - *(const unsigned char*)s2;
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
