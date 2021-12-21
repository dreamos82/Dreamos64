#include <stdio.h>
#include <stdarg.h>
#include <video.h>
#include <qemu.h>
#include <vsprintf.h>

#define is_digit(c)  ((c) >= '0' && (c) <= '9')

static size_t strnlen(const char *s, size_t count){
  const char *sc;
  for (sc = s; *sc != '\0' && count--; ++sc);
  return sc - s;
}

int printf(const char *fmt, ...){
    va_list arguments;
    char buffer[1024];
    va_start(arguments, fmt);
    vsprintf(buffer, fmt, arguments);
    va_end(arguments);
    _printStr(buffer);
    return 0;
}

static int skip_atoi(const char **s){
  int i = 0;
  while (is_digit(**s)) i = i*10 + *((*s)++) - '0';
  return i;
}

int vsprintf(char *buffer, const char *fmt, va_list args){
    char *str;
    for(str = buffer; *fmt; fmt++){
        int precision;
        if(*fmt != '%'){
            *str++ = *fmt; //fmt is increased by the for loop
            continue;
        }
        fmt++;
        precision = -1;
        if(*fmt == '.'){
            ++fmt;
            precision = skip_atoi(&fmt);
            if(precision < 0) {
                precision = 0;
            }            
        }
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
                int str_len = strnlen(arg_string, precision);
                for(int i=0; i < str_len; ++i) {
                    *str++ = *arg_string++;
                }
                /*while(*arg_string != 0){
                    *str++ = *arg_string++;
                }*/
                break;
            }
        }
    }

    *str = '\0';
    return str-buffer;
}
