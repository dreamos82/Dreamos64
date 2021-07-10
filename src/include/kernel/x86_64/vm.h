#ifndef _VM_H
#define _VM_H

#include <stdint.h>

#define PML4_ENTRY(address)((address>>39) & 0x1ff)
#define PDPR_ENTRY(address)((address>>30) & 0x1ff)
#define PD_ENTRY(address)((address>>21) & 0x1ff)
#define PT_ENTRY(address)((address>>12) & 0x1ff)
#define SIGN_EXTENSION 0xFFFF000000000000
#define ENTRIES_TO_ADDRESS(pml4, pdpr, pd, pt)((pml4 << 39) | (pdpr << 30) | (pd << 21) |  (pt << 12))

#define PRESENT_BIT 1
#define WRITE_BIT 0b10

#define VM_PAGES_PER_TABLE 0x200

#if SMALL_PAGES == 0
#define VM_OFFSET_MASK 0xFFFFFFFFFFE00000
#elif SMALL_PAGES == 1
#define VM_OFFSET_MASK 0xFFFFFFFFFFFFF000
#endif

void page_fault_handler(uint64_t);

void initialize_vm();


#endif
