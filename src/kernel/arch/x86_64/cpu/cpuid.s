section .text
global _cpuid
_cpuid:
    [bits 64]
    mov eax, 0x0
    cpuid
    mov [processor_string], ebx
    mov [processor_string + 4], edx
    mov [processor_string + 8], ecx
    mov byte[processor_string + 12], 0
    mov rax, processor_string
    ret

section .bss
processor_string:
    resb 13

