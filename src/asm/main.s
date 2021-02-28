global long_mode_start
extern kernel_start
section .text
bits 64
long_mode_start:
    ; mov rax, 0x2f592f412fb2f4f
    ; mov qword [0xb8000], rax:::


    mov word [0xb8000], 0x0248  ;Letter 'H'
    mov word [0xb8002], 0x0265  ;Letter 'e'
    mov word [0xb8004], 0x026c  ;Letter 'l'
    mov word [0xb8006], 0x026c  ;Letter 'l'
    mov word [0xb8008], 0x026f  ;Letter 'o'
    mov word [0xb800a], 0x022c  ;Letter ','
    mov word [0xb800c], 0x0220  ;Letter ' '
    mov word [0xb800e], 0x0244  ;Letter 'D'
    mov word [0xb8010], 0x0272  ;Letter 'r'
    mov word [0xb8012], 0x0265  ;Letter 'e'
    mov word [0xb8014], 0x0261  ;Letter 'a'
    mov word [0xb8016], 0x026d  ;Letter 'm'
    mov word [0xb8018], 0x024f  ;Letter 'O'
    mov word [0xb801a], 0x0273  ;Letter 's'
    mov word [0xb801c], 0x0236  ;Letter '6'
    mov word [0xb801e], 0x0234  ;Letter '4'
   
    ;mov rbx, header_start 
    ;push rbx 
    call kernel_start
    hlt
