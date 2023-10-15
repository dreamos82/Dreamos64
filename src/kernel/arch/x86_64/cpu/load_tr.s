section .text

global _load_task_register
_load_task_register:
    mov rax, 0x28
    ltr ax
