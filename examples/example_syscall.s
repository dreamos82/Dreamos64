extern loop
[bits 64]
loop:
    mov rdi, 0x63
    mov rsi, 0x2
    lea rdx, userspace_buffer
    int 0x80
    jmp loop

section .bss

align 4096
userspace_buffer:
    resb 20
