#ifndef _VM_H
#define _VM_H

#include <stdint.h>
#include <stdbool.h>
#include <bitmap.h>

#define PML4_ENTRY(address)((address>>39) & 0x1ff)
#define PDPR_ENTRY(address)((address>>30) & 0x1ff)
#define PD_ENTRY(address)((address>>21) & 0x1ff)
#define PT_ENTRY(address)((address>>12) & 0x1ff)
#define SIGN_EXTENSION 0xFFFF000000000000
#define ENTRIES_TO_ADDRESS(pml4, pdpr, pd, pt)((pml4 << 39) | (pdpr << 30) | (pd << 21) |  (pt << 12))

#define PAGE_ALIGNMENT_MASK (PAGE_SIZE_IN_BYTES-1)

#define ALIGN_PHYSADDRESS(address)(address & (~(PAGE_ALIGNMENT_MASK)))

#define HIGHER_HALF_ADDRESS_OFFSET 0xFFFF800000000000

#define PRESENT_BIT 1
#define WRITE_BIT 0b10
#define HUGEPAGE_BIT 0b10000000

#define VM_PAGES_PER_TABLE 0x200

#define PRESENT_VIOLATION   0x1
#define WRITE_VIOLATION 0x2
#define ACCESS_VIOLATION    0x4
#define RESERVED_VIOLATION   0x8
#define FETCH_VIOLATION 0x10

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

bool is_address_higher_half(uint64_t);
#endif
