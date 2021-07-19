%define MULTIBOOT_TAG_TYPE_END  0
%define MULTIBOOT_TAG_TYPE_BASIC_MEMINFO 4
%define MULTIBOOT_TAG_TYPE_MMAP 6
%define MULTIBOOT_TAG_TYPE_FRAMEBUFFER 8
%define MULTIBOOT_TAG_TYPE_LOAD_BASE_ADDR 21
%define MULTIBOOT_TAG_TYPE_ACPI_OLD 14
%define MULTIBOOT_TAG_TYPE_ACPI_NEW 15
struc multiboot_tag
    .type:  resd 1
    .size:  resd 1
endstruc

struc multiboot_tag_framebuffer
    .type:  resd 1
    .size:  resd 1
    .framebuffer_addr:  resq 1
    .framebuffer_pitch: resd 1
    .framebuffer_width: resd 1
    .framebuffer_height:    resd 1
    .framebuffer_bpp:   resb    1
    .framebuffer_type:  resb    1
    .reserved:  resb 1
endstruc
    
