#include <tss.h>

void initialize_tss(){

    tss_t kernel_tss;

    kernel_tss.reserved0 = 0x0;
    kernel_tss.reserved1 = 0x0;
    kernel_tss.reserved2 = 0x0;
    kernel_tss.reserved3 = 0x0;
    kernel_tss.rsp1 = 0x0;
    kernel_tss.rsp2 = 0x0;
    kernel_tss.ist1 = 0x0;
    kernel_tss.ist2 = 0x0;
    kernel_tss.ist3 = 0x0;
    kernel_tss.ist4 = 0x0;
    kernel_tss.ist5 = 0x0;
    kernel_tss.ist6 = 0x0;
    kernel_tss.ist7 = 0x0;
    kernel_tss.io_bitmap_offset = 0x0;
}
