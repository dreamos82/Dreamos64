#ifndef _VIDEO_H
#define _VIDEO_H

#define _SCR_W 80
#define _SCR_H 25

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
void _printNumber(char*, unsigned int, int);
void _printHex(char*, unsigned int);
void _clearScreen();

char* _getHexString(char*, unsigned int);
#endif
