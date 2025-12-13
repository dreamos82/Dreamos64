#include <framebuffer.h>
#include <idt.h>
#include <logging.h>
#include <rtc.h>
#include <syscalls.h>
#include <sys_read.h>

bool _syscalls_init() {
    pretty_log(Verbose, "Initializing sycalls");
    set_idt_entry(SYSCALL_VECTOR_NUMBER, IDT_PRESENT_FLAG | IDT_INTERRUPT_TYPE_FLAG | IDT_DPL_USER_FLAG, KERNEL_CS, 0, interrupt_service_routine_128);
    return true;
}

cpu_status_t *syscall_dispatch(cpu_status_t* regs) {
    //TODO: add mapping / unmapping memory syscall
    syscall_vector sc_num = regs->rdi;
    //pretty_logf(Verbose, "Syscall handler called: %d", sc_num);
    switch(sc_num) {
        case SYS_EXAMPLE:
            // sc_num 1 is reserved for tests purposes
            //_fb_printStrAndNumberAt("Epoch time: ", read_rtc_time(), 0, 11, 0xf5c4f1, 0x000000);
            _fb_printStrAt("Hello from user world (through a syscall...)", 0, 15, 0xf5c4f1, 0x000000);
            //pretty_log(Verbose, "example");
            break;
        case SYS_READ:
            //SYS_READ: This syscall reads input from the keyboard
            //Parameters required: - size, buffer
            int fildes = regs->rsi;
            uint64_t buffer = regs->rdx;
            size_t nbytes = regs->rcx;
            size_t bytes_read = sys_read(fildes, (void*)buffer, nbytes);
            regs->rax = bytes_read;
            _fb_printStrAndNumberAt("Userspace address: 0x", buffer, 0, 15, 0xf5c4f1, 0x000000);
            _fb_printStrAndNumberAt("nbytes: ", nbytes, 0, 16, 0xf5c4f1, 0x000000);
            break;
        case SYS_PRINT:
            //SYS_PRINT: This prints the buffer passed in `rsi`
            //TODO: Add position? Or handle position
            char *read_buffer = (char *)regs->rsi;
            size_t read_nbytes = regs->rdx;
            size_t pos_x = regs->rcx;
            size_t pos_y = regs->r8;
            read_buffer[read_nbytes-1] = '\0';
            if (pos_x == 0 && pos_y == 0) {
                _fb_printStr(read_buffer, 0x27F549, 0x000000);
                _fb_printStr("The line above is printed after the read syscall.", 0xD3F527, 0x000000);
            } else {
                _fb_printStrAt(read_buffer, pos_x, pos_y, 0x27F549, 0x000000);
                _fb_printStrAt("The line above is printed after the read syscall.", pos_x, pos_y + 1, 0xD3F527, 0x000000);
            }
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

