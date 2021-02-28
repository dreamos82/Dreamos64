section .multiboot_header
header_start:
	align 8
    dd 0xe85250d6   ;magic_number
    dd 0            ;Protected mode
    dd header_end - header_start    ;Header length

    ;compute checksum
    dd 0x100000000 - (0xe85250d6 + 0 + (header_end - header_start))

    ;here ends the required part of the multiboot header
	;The following is the end tag, must be always present
    ;end tag
    align 8
    dw 0    ;type
    dw 0    ;flags
    dd 8    ;size
header_end:


