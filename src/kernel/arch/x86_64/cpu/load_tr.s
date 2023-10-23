section .text

; This function just  load the tss selecto in the task _load_task_register
; it should be called only once during kernel initialization.
global _load_task_register
_load_task_register:
    mov rax, 0x28
    ltr ax
    ret
