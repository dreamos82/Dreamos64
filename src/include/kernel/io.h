#ifndef _IO_H
#define _IO_H


#define cli() __asm__ __volatile__("cli");
#define sti() __asm__ __volatile__("sti");

extern unsigned char inportb (int);
extern void outportb (int, unsigned char);


#endif
