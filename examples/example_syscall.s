extern loop
[bits 64]
loop:
    mov rdi, 0x63
    mov rsi, 0x1
    int 0x80
    jmp loop
