section .multiboot_header
header_start:
	align 8
    dd 0xe85250d6   ;magic_number
    dd 0            ;Protected mode
    dd header_end - header_start    ;Header length

    ;compute checksum
    dd 0x100000000 - (0xe85250d6 + 0 + (header_end - header_start))

framebuffer_tag_start:
    dw  0x05    ;Type: framebuffer
    dw  0x01    ;Optional tag
    dd  framebuffer_tag_end - framebuffer_tag_start ;size
    dd  0   ;Width - if 0 we let the bootloader decide
    dd  0   ;Height - same as above
    dd  0   ;Depth  - same as above
framebuffer_tag_end:

    ;here ends the required part of the multiboot header
	;The following is the end tag, must be always present
    ;end tag
    align 8
    dw 0    ;type
    dw 0    ;flags
    dd 8    ;size
header_end:


