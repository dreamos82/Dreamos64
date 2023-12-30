#ifndef __SYSCALLS_H__
#define __SYSCALLS_H__

#include <cpu.h>

#define SYSCALL_VECTOR_NUMBER 0x80

bool _syscalls_init();
cpu_status_t *do_syscall(cpu_status_t* regs);
void _sc_putc(char ch, size_t arg);

#endif
