#ifndef _VIDEO_H
#define _VIDEO_H

#include <stdint.h> 

#define _SCR_W 80
#define _SCR_H 25

#define _VIDEO_MEM_START 0xFFFFFFFF800b8000
typedef enum {
    BLACK,
    BLUE,
    GREEN,
    CYAN,
    RED,
    MAGENTA,
    BROWN,
    GREY,
    DARK_GREY,
    BRIGHT_BLUE,
    BRIGHT_GREEN,
    BRIGHT_CYAN,
    BRIGHT_RED,
    BRIGHT_MAGENTA,
    YELLOW,
    WHITE
} character_color;

void _printCh(char, character_color);
void _printStr(char *);
void _printNewLine();
void _printNumber(char*, unsigned long, int);
void _printHex(char*, unsigned long);
void _clearScreen();
void _printStringAndNumber(char *, unsigned long);
void _scrollUp();

int _getLineNumber();
#endif
