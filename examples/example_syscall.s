extern loop
[bits 64]
loop:
    mov rdi, 0x2 ; Syscall number
    lea rsi, userspace_buffer ; Read buffer
    mov rdx, 0x10 ; Size of read
    int 0x80 ; Syscall
    jmp loop

section .bss

align 4096
userspace_buffer:
    resb 20
