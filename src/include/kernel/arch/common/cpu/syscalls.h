#ifndef __SYSCALLS_H__
#define __SYSCALLS_H__

#include <cpu.h>

#define SYSCALL_VECTOR_NUMBER 0x80

#define E_NO_SYSCALL    -1

typedef enum {
    SYS_ZERO,
    SYS_EXAMPLE,
    SYS_PRINT,
    SYS_OPEN,
    SYS_READ
} syscall_vector;

bool _syscalls_init();
cpu_status_t *syscall_dispatch(cpu_status_t* regs);
size_t execute_syscall( size_t syscall_num, size_t  arg0, size_t arg1, size_t arg2 );

#endif
