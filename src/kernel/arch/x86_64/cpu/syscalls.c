#include <idt.h>
#include <logging.h>
#include <syscalls.h>

bool _syscalls_init() {
    pretty_log(Verbose, "Initializing sycalls");
    set_idt_entry(SYSCALL_VECTOR_NUMBER, IDT_PRESENT_FLAG | IDT_INTERRUPT_TYPE_FLAG | IDT_DPL_USER_FLAG, KERNEL_CS, 0, interrupt_service_routine_128);
    return true;
}

cpu_status_t *syscall_dispatch(cpu_status_t* regs) {
    size_t sc_num = regs->rsi;
    pretty_logf(Verbose, "Syscall handler called: %d", sc_num);
    switch(sc_num) {
        case 1:
            pretty_log(Verbose, "example");
            break;
        case 2:
            //char *input_string = (char *) regs->rsi;
            //pretty_logf(Verbose, "%s", input_string);
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
