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
#define HUGEPAGE_BIT 0b10000000

#define VM_PAGES_PER_TABLE 0x200

#if SMALL_PAGES == 0
#define VM_OFFSET_MASK 0xFFFFFFFFFFE00000
#define PAGE_ENTRY_FLAGS PRESENT_BIT | WRITE_BIT | HUGEPAGE_BIT
#elif SMALL_PAGES == 1
#define VM_OFFSET_MASK 0xFFFFFFFFFFFFF000
#define PAGE_ENTRY_FLAGS PRESENT_BIT | WRITE_BIT
#endif

void page_fault_handler(uint64_t);

void initialize_vm();

void clean_new_table(uint64_t *);

void invalidate_page_table(uint64_t *);
uint64_t ensure_address_in_higher_half( uint64_t );
#endif
