extern main
[bits 64]
main:
    mov rdi, 0x2 ; Read syscall
    mov rsi, 0x0 ; File Descriptor
    lea rdx, userspace_buffer ; Read buffer
    mov rcx, 0x8 ; Size of read
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
