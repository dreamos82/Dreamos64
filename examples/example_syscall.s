extern loop
[bits 64]
loop:
    mov rdi, 0x2 ; Syscall number
    lea rsi, userspace_buffer ; Read buffer
    mov rdx, 0x6 ; Size of read
    int 0x80 ; Syscall
loop1:
    mov rdi, 0x3
    lea rsi, userspace_buffer
    mov rdx, 0x6
    int 0x80;
    jmp loop1

section .bss

align 4096
userspace_buffer:
    resb 20
