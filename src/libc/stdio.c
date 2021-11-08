#include <stdio.h>
#include <stdarg.h>
#include <video.h>
#include <qemu.h>

int printf(const char *fmt, ...){
    va_list arguments;
    char buffer[1024];
    va_start(arguments, fmt);
    vsprintf(buffer, fmt, arguments);
    va_end(arguments);
    _printStr(buffer);
    return 0;
}

int vsprintf(char *buffer, const char *fmt, va_list args){
    char *str;
    for(str = buffer; *fmt; fmt++){
        if(*fmt != '%'){
            *str++ = *fmt; //fmt is increased by the for loop
            continue;
        }
        fmt++;
        switch(*fmt){
            case 'd':{
                int number = va_arg(args, int);
                int string_size = _getDecString(str, number);
                str+= string_size;
                break;
            }
            case 'x':{
                int number = va_arg(args, int);
                int string_size = _getHexString(str, number);
                str+= string_size; 
                break;
            }
            case 'c':
                break;
            case 's':{
                char *arg_string = va_arg(args, char*);
//                qemu_write_string(arg_string[0]);
                while(*arg_string != 0){
                    *str++ = *arg_string++;
                }
                break;
            }
        }
    }

    *str = '\0';
    return str-buffer;
}
