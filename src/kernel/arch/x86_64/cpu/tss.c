#include <tss.h>

tss_t kernel_tss;

void initialize_tss(){

    // These fields are reserved and must be set to 0
    kernel_tss.reserved0 = 0x0;
    kernel_tss.reserved1 = 0x0;
    kernel_tss.reserved2 = 0x0;
    kernel_tss.reserved3 = 0x0;

    // The rspX are used when there is a privilege change from a lower to a higher privilege
    // Rsp contain the stack for that privilege level.
    // We use only privilege level 0 and 3, so rsp1 and rsp2 can be left as 0
    // Every thread will have it's own rsp0 pointer
    kernel_tss.rsp0 = 0x0; // this will not be 0
    kernel_tss.rsp1 = 0x0;
    kernel_tss.rsp2 = 0x0;
    // istX are the Interrup stack table,  unlesse some specific cases they can be left as 0
    // See intel manual chapter 5
    kernel_tss.ist1 = 0x0;
    kernel_tss.ist2 = 0x0;
    kernel_tss.ist3 = 0x0;
    kernel_tss.ist4 = 0x0;
    kernel_tss.ist5 = 0x0;
    kernel_tss.ist6 = 0x0;
    kernel_tss.ist7 = 0x0;
    // Ca nbe left as 0 for now
    kernel_tss.io_bitmap_offset = 0x0;
}
