[bits  64]
; TODO: push exception code
[extern interrupts_handler]

%macro interrupt_service_routine 1
[global interrupt_service_routine_%1]
interrupt_service_routine_%1:
    cli
    push 0	; since we have no error code, to keep things consistent we push a default EC of 0
    push %1 ; pushing the interrupt number for easier identification by the handler
	save_context
	mov rdi, %1
    cld
    call interrupts_handler
	restore_context
    iretq
%endmacro

%macro interrupt_service_routine_error_code 1
[global interrupt_service_routine_error_code_%1]
interrupt_service_routine_error_code_%1:
    cli
    push %1 ; In this case the error code is already present on the stack
	save_context
	mov rdi, %1
    cld
    call interrupts_handler
	restore_context
    iretq
%endmacro



%macro save_context 0
    push rax
    push rcx
    push rdx
    push rbx
    push rsp
    push rbp
    push rsi
    push rdi
    push r8
    push r9
    push r10
    push r11
    push r12
    push r13
    push r14
    push r15
%endmacro

%macro restore_context 0
    pop r15
    pop r14
    pop r13
    pop r12
    pop r11
    pop r10
    pop r9
    pop r8
    pop rdi
    pop rsi
    pop rsi
    pop rbp
    pop rsp
    pop rbx
    pop rdx
    pop rcx
    pop rax
%endmacro

interrupt_service_routine_error_code 14
