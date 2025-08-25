#include <framebuffer.h>
#include <idt.h>
#include <logging.h>
#include <rtc.h>
#include <syscalls.h>

bool _syscalls_init() {
    pretty_log(Verbose, "Initializing sycalls");
    set_idt_entry(SYSCALL_VECTOR_NUMBER, IDT_PRESENT_FLAG | IDT_INTERRUPT_TYPE_FLAG | IDT_DPL_USER_FLAG, KERNEL_CS, 0, interrupt_service_routine_128);
    return true;
}

cpu_status_t *syscall_dispatch(cpu_status_t* regs) {
    //TODO: add mapping / unmapping memory syscall
    size_t sc_num = regs->rsi;
    //pretty_logf(Verbose, "Syscall handler called: %d", sc_num);
    switch(sc_num) {
        case 1:
            // sc_num 1 is reserved for tests purposes
            //_fb_printStrAndNumberAt("Epoch time: ", read_rtc_time(), 0, 11, 0xf5c4f1, 0x000000);
            _fb_printStrAt("Hello from user world (through a syscall...)", 0, 15, 0xf5c4f1, 0x000000);
            //pretty_log(Verbose, "example");
            break;
        case 2:
            //char *input_string = (char *) regs->rsi;
            //pretty_logf(Verbose, "%s", input_string);
            //This will call sys_read
            //Parameters required: - size, buffer
            //pretty_logf(Verbose, "Userspace buffer: 0x%x", regs->rdx);
            //TODO: call sys_read
            uint64_t control_value = regs->rdi;
            size_t nbytes = regs->rsi;
            void *read_buffer = (void *)regs->rdx;
            _fb_printStrAndNumberAt("Hello from user world (through a syscall...) ", control_value, 0, 15, 0xf5c4f1, 0x000000);
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

