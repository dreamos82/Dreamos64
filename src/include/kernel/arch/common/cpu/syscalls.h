#ifndef __SYSCALLS_H__
#define __SYSCALLS_H__

#include <cpu.h>

#define SYSCALL_VECTOR_NUMBER 0x80

bool _syscalls_init();
cpu_status_t *do_syscall(cpu_status_t* regs);
//void _sc_putc(char ch, size_t arg);
size_t syscall_dispatcher( size_t syscall_num, size_t  arg0, size_t arg1, size_t arg2 );

#endif
