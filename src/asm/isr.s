[bits  64]

[extern int_14]

%macro interrupt_service_routine 1
[global interrupt_service_routine_%1]
interrupt_service_routine_%1:
	cli
	push rax
	push rcx
	push rdx
	push rbx
	push rsp
	push rbp
	push rsi
	push rdi
	cld
	call int_14
	pop rdi
	pop rsi
	pop rbp
	pop rsp
	pop rbx
	pop rdx
	pop rcx
	pop rax	
	iretq
%endmacro

interrupt_service_routine 14
