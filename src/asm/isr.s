[bits  64]
[extern interrupts_handler]

%macro interrupt_service_routine 1
[global interrupt_service_routine_%1]
interrupt_service_routine_%1:
    ; When this macro is called the status registers are already on the stack
    push 0	; since we have no error code, to keep things consistent we push a default EC of 0
    push %1 ; pushing the interrupt number for easier identification by the handler
    save_context ; Now we can save the general purpose registers
    mov rdi, rsp    ; Let's set the current stack pointer as a parameter of the interrupts_handler
    cld ; Clear the direction flag
    call interrupts_handler ; Now we call the interrupt handler
    mov rsp, rax    ; use the returned context
    restore_context ; We served the interrupt let's restore the previous context
    add rsp, 16 ; We can discard the interrupt number and the error code
    iretq ; Now we can return from the interrupt
%endmacro

%macro interrupt_service_routine_error_code 1
[global interrupt_service_routine_error_code_%1]
interrupt_service_routine_error_code_%1:
    push %1 ; In this case the error code is already present on the stack
    save_context
    mov rdi, rsp
    cld
    call interrupts_handler
    restore_context
    add rsp, 16
    iretq
%endmacro

%macro save_context 0
    push rax
    push rbx
    push rcx
    push rdx
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
    pop rbp
    pop rdx
    pop rcx
    pop rbx
    pop rax
%endmacro

interrupt_service_routine 0
interrupt_service_routine 1
interrupt_service_routine 2
interrupt_service_routine 3
interrupt_service_routine 4
interrupt_service_routine 5
interrupt_service_routine 6
interrupt_service_routine 7
interrupt_service_routine_error_code 8
interrupt_service_routine 9
interrupt_service_routine_error_code 10
interrupt_service_routine_error_code 11
interrupt_service_routine_error_code 12
interrupt_service_routine_error_code 13
interrupt_service_routine_error_code 14
interrupt_service_routine 15
interrupt_service_routine 16
interrupt_service_routine_error_code 17
interrupt_service_routine 18
interrupt_service_routine 32
interrupt_service_routine 33
interrupt_service_routine 34
interrupt_service_routine 128
interrupt_service_routine 255
