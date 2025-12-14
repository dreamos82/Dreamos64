extern main
[bits 64]
main:
    mov rdi, 0x3 ; Print syscall
    mov rsi, message_string ; String to print
    mov rdx, 20 ; Size of string
    mov rcx, 0 ; xpos
    mov r8, 18 ; ypos
    int 0x80
    mov rdi, 0x2 ; Read syscall
    mov rsi, 0x0 ; File Descriptor
    lea rdx, userspace_buffer ; Read buffer
    mov rcx, 0x8 ; Size of read
    int 0x80 ; Syscall
    cmp rax, -1
    je read_error
    mov byte [rdx + rax], 0
    jmp loop1
read_error:
    mov rdi, 0x3 ; Print syscall
    mov rsi, error_string ; Print buffer
    mov rdx, 20
    int 0x80
    jmp read_error
loop1:
    mov rdi, 0x3 ; Print syscall
    lea rsi, userspace_buffer ; Print buffer
    mov rdx, 0x8 ; Size of print
    mov rcx, 0 ;xpos (optional)
    mov r8, 25 ;ypos (optional)
    int 0x80 ; Syscall
    jmp loop1


section .data

error_string db "Keyboard read error", 0
message_string db "Enter your text:", 0


section .bss

align 4096
userspace_buffer:
    resb 20
