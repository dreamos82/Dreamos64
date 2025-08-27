extern loop
[bits 64]
loop:
    mov rdi, 0x2
    lea rsi, userspace_buffer
    mov rdx, 0x10
    int 0x80
    jmp loop

section .bss

align 4096
userspace_buffer:
    resb 20
