%define KERNEL_VIRTUAL_ADDR 0xFFFFFFFF80000000
%define PAGE_DIR_ENTRY_FLAGS 0b11
%define PRESENT_BIT 1
%define WRITE_BIT 0b10
%define HUGEPAGE_BIT 0b10000000
%if SMALL_PAGES == 1
%define PAGE_SIZE 0x1000
%define PAGE_TABLE_ENTRY WRITE_BIT | PRESENT_BIT
%define LOOP_LIMIT 1024
%elif SMALL_PAGES == 0
%define PAGE_SIZE 0x200000
%define PAGE_TABLE_ENTRY HUGEPAGE_BIT | WRITE_BIT | PRESENT_BIT
%define LOOP_LIMIT 512
%endif
section .multiboot.text
global start
global p2_table
extern kernel_start

[bits 32]

start:
    mov edi, ebx ; Address of multiboot structure
    mov esi, eax ; Magic number

    mov esp, stack.top - KERNEL_VIRTUAL_ADDR
    
    ; For now we are goin to use 2Mib pages 
    ; We need only 3 table levels instead of 4
    mov eax, p3_table - KERNEL_VIRTUAL_ADDR; Copy p3_table address in eax
    or eax, PRESENT_BIT | WRITE_BIT        ; set writable and present bits to 1
    mov dword [(p4_table - KERNEL_VIRTUAL_ADDR) + 0], eax   ; Copy eax content into the entry 0 of p4 table

    mov eax, p3_table_hh - KERNEL_VIRTUAL_ADDR
    or eax, PRESENT_BIT | WRITE_BIT
    mov dword [(p4_table - KERNEL_VIRTUAL_ADDR) + 511 * 8], eax

    mov eax, p4_table - KERNEL_VIRTUAL_ADDR ; Mapping the PML4 into itself
    or eax, PRESENT_BIT | WRITE_BIT
    mov dword [(p4_table - KERNEL_VIRTUAL_ADDR) + 510 * 8], eax

    mov eax, p2_table - KERNEL_VIRTUAL_ADDR  ; Let's do it again, with p2_table
    or eax, PRESENT_BIT | WRITE_BIT       ; Set the writable and present bits
    mov dword [(p3_table - KERNEL_VIRTUAL_ADDR) + 0], eax   ; Copy eax content in the 0th entry of p3

    mov eax, p2_table - KERNEL_VIRTUAL_ADDR
    or eax, PRESENT_BIT | WRITE_BIT
    mov dword[(p3_table_hh - KERNEL_VIRTUAL_ADDR) + 510 * 8], eax
    %if SMALL_PAGES == 1
    mov ebx, 0
    mov eax, pt_tables - KERNEL_VIRTUAL_ADDR
    .map_pd_table:
        or eax, PRESENT_BIT | WRITE_BIT
        mov dword[(p2_table - KERNEL_VIRTUAL_ADDR) + ebx * 8], eax
        add eax, 0x1000
        inc ebx
        cmp ebx, 2
        jne .map_pd_table
    %endif
    ; Now let's prepare a loop...
    mov ecx, 0  ; Loop counter

    .map_p2_table:
        mov eax, PAGE_SIZE  ; Size of the page
        mul ecx             ; Multiply by counter
        or eax, PAGE_TABLE_ENTRY ; We set: huge page bit, writable and present 

        ; Moving the computed value into p2_table entry defined by ecx * 8
        ; ecx is the counter, 8 is the size of a single entry
        %if SMALL_PAGES == 1
        mov [(pt_tables - KERNEL_VIRTUAL_ADDR) + ecx * 8], eax
        %elif SMALL_PAGES == 0
        mov [(p2_table - KERNEL_VIRTUAL_ADDR) + ecx * 8], eax
        %endif

        inc ecx             ; Let's increase ecx
        cmp ecx, LOOP_LIMIT        ; have we reached 512 ?
                            ; each table is 4k size. Each entry is 8bytes
                            ; that is 512 entries in a table
        
        jne .map_p2_table   ; if ecx < 512 then loop

    ; This section is temporary, is here only to test the framebuffer features!
    ; Will be removed once the the memory management will be implemented
    ;mov eax, fbb_p2_table - KERNEL_VIRTUAL_ADDR
    ;or eax, PRESENT_BIT | WRITE_BIT
    ;mov dword [(p3_table - KERNEL_VIRTUAL_ADDR)+ 8 * 3], eax

    ;This section will be removed
    ;mov eax, 0xFD000000
    ;or eax, 0b10000011
    ;mov dword [(fbb_p2_table - KERNEL_VIRTUAL_ADDR) + 8 * 488], eax
    %if SMALL_PAGES == 0
    mov eax, 0xFD000000
    or eax, PAGE_TABLE_ENTRY
    mov dword [(p2_table - KERNEL_VIRTUAL_ADDR) + 8 * 488], eax
    %endif
    ; All set... now we are nearly ready to enter into 64 bit
    ; Is possible to move into cr3 only from another register
    ; So let's move p4_table address into eax first
    ; then into cr3
    mov eax, (p4_table - KERNEL_VIRTUAL_ADDR)
    mov cr3, eax

    ; Now we can enable PAE
    ; To do it we need to modify cr4, so first let's copy it into eax
    ; we can't modify cr registers directly
    mov eax, cr4
    or eax, 1 << 5  ; Physical address extension bit
    mov cr4, eax
    
    ; Now set up the long mode bit
    mov ecx, 0xC0000080
    ; rdmsr is to read a a model specific register (msr)
    ; it copy the values of msr into eax
    rdmsr
    or eax, 1 << 8
    ; write back the value
    wrmsr
    
    ; Now is tiem to enable paging
    mov eax, cr0    ;cr0 contains the values we want to change
    or eax, 1 << 31 ; Paging bit
    or eax, 1 << 16 ; Write protect, cpu  can't write to read-only pages when
                    ; privilege level is 0
    mov cr0, eax    ; write back cr0
    ; load gdt 
    lgdt [gdt64.pointer_low - KERNEL_VIRTUAL_ADDR]
    jmp (0x8):(kernel_jumper - KERNEL_VIRTUAL_ADDR)
    bits 64

section .text
kernel_jumper:
    bits 64    

    ; update segment selectors
    mov ax, 0x10
    mov ss, ax  ; Stack segment selector
    mov ds, ax  ; data segment register
    mov es, ax  ; extra segment register
    mov fs, ax  ; extra segment register
    mov gs, ax  ; extra segment register

    mov rax, higher_half
    jmp rax

higher_half:
    ; Far jump to long mode
    mov rsp, stack.top
    lgdt [gdt64.pointer]

    ; The two lines below are needed to un map the kernel in the lower half
    ; But i'll leave them commented for now because the code in the kernel need 
    ; to be changed and some addresses need to be updated (i.e. multiboot stuff)
    ;mov eax, 0x0
    ;mov dword [(p4_table - KERNEL_VIRTUAL_ADDR) + 0], eax
    call kernel_start

section .bss

align 4096
p4_table: ;PML4
    resb 4096
p3_table: ;PDPR
    resb 4096
p3_table_hh: ;PDPR
    resb 4096 
p2_table: ;PDP
    resb 4096

%if SMALL_PAGES == 1
; if SMALL_PAGES is defined it means we are using 4k pages
; For now the first 8mb will be mapped for the kernel.
pt_tables:
    resb 8192
fdd_pt_tables:
    resb 8192
%endif
; This section is temporary to test the framebuffer
align 4096
fbb_p2_table:
    resb 4096

stack:
    resb 16384
    .top:

section .rodata

; gdt table needs at least 3 entries:
;     the first entry is always null
;     the other two are data segment and code segment.
gdt64:
    dq  0	;first entry = 0
    .code equ $ - gdt64
        ; set the following values:
        ; descriptor type: bit 44 has to be 1 for code and data segments
        ; present: bit 47 has to be  1 if the entry is valid
        ; read/write: bit 41 1 means that is readable
        ; executable: bit 43 it has to be 1 for code segments
        ; 64bit: bit 53 1 if this is a 64bit gdt
        dq (1 <<44) | (1 << 47) | (1 << 41) | (1 << 43) | (1 << 53)  ;second entry=code=8
    .data equ $ - gdt64
        dq (1 << 44) | (1 << 47) | (1 << 41)	;third entry = data = 10

.pointer:
    dw .pointer - gdt64 - 1
    dq gdt64
.pointer_low:
    dw .pointer - gdt64 - 1
    dq gdt64 - KERNEL_VIRTUAL_ADDR


