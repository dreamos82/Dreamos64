section .text
global _cpuid_model
_cpuid_model:
    [bits 64]
    mov eax, 0x0
    cpuid
    mov [processor_string], ebx
    mov [processor_string + 4], edx
    mov [processor_string + 8], ecx
    mov byte[processor_string + 12], 0
    mov rax, processor_string
    ret

;Maybe just return edx in the future
;(ignoring ecx for now)
global _cpuid_feature_apic
_cpuid_feature_apic:
    mov rax, 0x1
    cpuid
    and edx, 0x100
    mov eax, edx
    ret
   
    

section .bss
processor_string:
    resb 13

