%define MULTIBOOT_TAG_TYPE_END  0
%define MULTIBOOT_TAG_TYPE_BASIC_MEMINFO 4
%define MULTIBOOT_TAG_TYPE_MMAP 6
%define MULTIBOOT_TAG_TYPE_FRAMEBUFFER 8
%define MULTIBOOT_TAG_TYPE_LOAD_BASE_ADDR 21

struc multiboot_tag
    .type:   resd 1
    .size:   resd 1
endstruc