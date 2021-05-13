#ifndef _VM_H
#define _VM_H

#include <stdint.h>

#if SMALL_PAGES == 0
#define VM_OFFSET_MASK 0xFFFFFFFFFFE00000
#define VM_AND_MASK 0x1FF
#elif SMALL_PAGES == 1
#define VM_OFFSET_MASK 0xFFFFFFFFFFFFF000
#define VM_AND_MASK 0x1FF //TO BE COMPUTED
#endif

void page_fault_handler(uint64_t);

void initialize_vm();


#endif
