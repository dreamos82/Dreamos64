#include <idt.h>
#include <logging.h>
#include <syscalls.h>

bool _syscalls_init() {
    pretty_log(Verbose, "Initializing sycalls");
    set_idt_entry(SYSCALL_VECTOR_NUMBER, IDT_PRESENT_FLAG | IDT_INTERRUPT_TYPE_FLAG | IDT_DPL_USER_FLAG, KERNEL_CS, 0, interrupt_service_routine_128);
    return false;
}

cpu_status_t *do_syscall(cpu_status_t* regs) {
    size_t sc_num = regs->rsi;
    pretty_logf(Verbose, "Syscall handler called: %d", sc_num);
    switch(sc_num) {
        case 1:
            pretty_log(Verbose, "example");
            break;
    }
    return regs;
}

/*void _sc_putc(char ch, size_t arg) {
    size_t syscall_num = 1;
    asm("int $0x80"
        : "+S"(syscall_num)
        : "D"(ch), "S"(arg)
    );
}*/
