#include <framebuffer.h>
#include <idt.h>
#include <logging.h>
#include <rtc.h>
#include <scheduler.h>
#include <syscalls.h>
#include <sys_read.h>
#include <sys_open.h>
#include <sys_print.h>
#include <sys_exit.h>

bool _syscalls_init() {
    pretty_log(Verbose, "Initializing sycalls");
    set_idt_entry(SYSCALL_VECTOR_NUMBER, IDT_PRESENT_FLAG | IDT_INTERRUPT_TYPE_FLAG | IDT_DPL_USER_FLAG, KERNEL_CS, 0, interrupt_service_routine_128);
    return true;
}

cpu_status_t *syscall_dispatch(cpu_status_t* regs) {
    syscall_vector sc_num = regs->rdi;
    //pretty_logf(Verbose, "Syscall handler called: %d", sc_num);
    switch(sc_num) {
        case SYS_EXAMPLE:
            // sc_num 1 is reserved for tests purposes
            //_fb_printStrAndNumberAt("Epoch time: ", read_rtc_time(), 0, 11, 0xf5c4f1, 0x000000);
            _fb_printStrAt("Hello from user world (through a syscall...)", 0, 15, 0xf5c4f1, 0x000000);
            //pretty_log(Verbose, "example");
            break;
        case SYS_OPEN:
            pretty_logf(Verbose, "NOT IMPLEMENTED SYSCALL: %d", sc_num);
            char *path = (char *) regs->rsi;
            size_t flags = regs->rdx;
            sys_open(path, flags);
            break;
        case SYS_READ:
            //SYS_READ: This syscall reads input from the keyboard
            //Parameters required: - size, buffer
            int fildes = regs->rsi;
            uint64_t buffer = regs->rdx;
            size_t nbytes = regs->rcx;
            size_t bytes_read = sys_read(fildes, (void*)buffer, nbytes);
            regs->rax = bytes_read;
            break;
        case SYS_PRINT:
            //SYS_PRINT: This prints the buffer passed in `rsi`
            //TODO: Add position? Or handle position
            char *print_buffer = (char *)regs->rsi;
            size_t print_nbytes = regs->rdx;
            size_t pos_x = regs->rcx;
            size_t pos_y = regs->r8;
            sys_print(print_buffer, print_nbytes, pos_x, pos_y);
            break;
        case SYS_EXIT:
            pretty_log(Info, "sys exit called");
            sys_thread_exit((int) regs->rsi);
            regs = schedule(regs);
            break;
        default:
            regs->rax = E_NO_SYSCALL;
            break;
    }
    return regs;
}

size_t execute_syscall( size_t syscall_num, size_t  arg0, size_t arg1, size_t arg2 ) {
    asm("int $0x80"
        : "=S"(arg2)
        : "D"(syscall_num), "S"(arg0), "rdx"(arg1), "rcx"(arg2)
    );
    return arg2;
}

