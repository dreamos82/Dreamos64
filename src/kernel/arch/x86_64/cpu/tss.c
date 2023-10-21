#include <logging.h>
#include <tss.h>

extern uint64_t gdt64[];
extern uint64_t stack;

tss_t kernel_tss;

void initialize_tss(){

    loglinef(Verbose, "(%s) Initializing tss", __FUNCTION__);

    // These fields are reserved and must be set to 0
    kernel_tss.reserved0 = 0x00;
    kernel_tss.reserved1 = 0x00;
    kernel_tss.reserved2 = 0x00;
    kernel_tss.reserved3 = 0x00;
    kernel_tss.reserved4 = 0x00;

    // The rspX are used when there is a privilege change from a lower to a higher privilege
    // Rsp contain the stack for that privilege level.
    // We use only privilege level 0 and 3, so rsp1 and rsp2 can be left as 0
    // Every thread will have it's own rsp0 pointer
    kernel_tss.rsp0 = 0x0;
    kernel_tss.rsp1 = 0x0;
    kernel_tss.rsp2 = 0x0;
    // istX are the Interrup stack table,  unless some specific cases they can be left as 0
    // See intel manual chapter 5
    kernel_tss.ist1 = 0x0;
    kernel_tss.ist2 = 0x0;
    kernel_tss.ist3 = 0x0;
    kernel_tss.ist4 = 0x0;
    kernel_tss.ist5 = 0x0;
    kernel_tss.ist6 = 0x0;
    kernel_tss.ist7 = 0x0;
    // Can be left as 0 for now
    kernel_tss.io_bitmap_offset = 0x0;
}

void load_tss() {
    // Fields explanation (each entry is 64bit)
    // TYPE: 1001 (64Bit TSS Available)
    // BASE_ADDRESS: kernel_tss
    // LIMIT 16:19 0 DPL: 0 P: 1 G:0

    gdt64[TSS_ENTRY_LOW] = 0x00;
    gdt64[TSS_ENTRY_HIGH] = 0x00;

    // TSS_ENTRY_LOW:
    uint16_t limit_low = (uint16_t) sizeof(kernel_tss); // 0:15 -> Limit (first 15 bits) should be 0xFFFF
    uint16_t tss_entry_base_1 = (((uint64_t)&kernel_tss & 0xFFFF));     // 16:31 -> First 16 bits of kernel_tss address
    uint8_t tss_entry_base_2 = (((uint64_t)&kernel_tss >> 16) & 0xFF); // 32:39 -> Next 8 bits of kernel_tss address
    uint8_t  flags_1 = 0x89; // 40:47 -> Type 4 bits in our case is 1001, 0,  DPL should be 0 , P = 1
    uint8_t flags_2 = 0; // 48:55 -> Limit (last 4 bits) can be 0, AVL=available to OS we leave it as 0, 53:54 are 0, 55 G (Granularity)
    uint8_t tss_entry_base_3 = (((uint64_t)&kernel_tss >> 24) & 0xFF);     // 55:63 -> Bits 25:31 of the kernel_tss base address
    // TSS_ENTRY_HIGH
    uint32_t tss_entry_base_4 = (((uint64_t) &kernel_tss>>32)& 0xFFFFFFFF); // 0:31 -> kernel_tss bits 32:63
    uint32_t reserved_part = 0; // 32:63 -> Reserved / 0

    uint64_t entry_low = (uint64_t) tss_entry_base_3 << 56 | (uint64_t) flags_2 << 48 | (uint64_t) flags_1 << 40 | (uint64_t) tss_entry_base_2 << 32| (uint64_t)tss_entry_base_1 << 16 | (uint64_t) limit_low;
    uint64_t entry_high = reserved_part | tss_entry_base_4;


    gdt64[TSS_ENTRY_LOW] = entry_low;
    gdt64[TSS_ENTRY_HIGH] = entry_high;
    loglinef(Verbose, "(%s) Loading TSS Register", __FUNCTION__);
    loglinef(Verbose, "(%s) kernel_tss address = 0x%x", __FUNCTION__, &kernel_tss);
    loglinef(Verbose, "(%s) gdt64[4]n = 0x%x", __FUNCTION__, entry_low);
    loglinef(Verbose, "(%s) gdt64[5]n = 0x%x", __FUNCTION__,  entry_high);
    loglinef(Verbose, "(%s) gdt64[4] = 0x%x", __FUNCTION__, (uint64_t)gdt64[TSS_ENTRY_LOW]);
    loglinef(Verbose, "(%s) gdt64[5] = 0x%x", __FUNCTION__,  (uint64_t)gdt64[TSS_ENTRY_HIGH]);

    _load_task_register();
}
