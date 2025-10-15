extern main
[bits 64]
main:
    mov rdi, 0x2 ; Read syscall
    lea rsi, userspace_buffer ; Read buffer
    mov rdx, 0x8 ; Size of read
    int 0x80 ; Syscall
loop1:
    mov rdi, 0x3 ; Print syscall
    lea rsi, userspace_buffer ; Print buffer
    mov rdx, 0x8 ; Size of print
    int 0x80 ; Syscall
    jmp loop1

section .bss

align 4096
userspace_buffer:
    resb 20
