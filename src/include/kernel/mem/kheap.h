#ifndef __KHEAP__H
#define __KHEAP__H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

#define KERNEL_MEMORY_PADDING 0x1000
#if SMALL_PAGES == 0
    #define KERNEL_PAGE_SIZE 0x200000
#elif SMALL_PAGES == 1
    #define KERNEL_PAGE_SIZE 0x1000
#endif

#define HEAP_ALLOC_ALIGNMENT 0x10
#define KHEAP_MINIMUM_ALLOCABLE_SIZE 0x20

typedef struct KHeapMemoryNode {
    uint64_t size;
    bool is_free;
    struct KHeapMemoryNode *next;
    struct KHeapMemoryNode *prev;
} KHeapMemoryNode;

//Service functions
void initialize_kheap();
KHeapMemoryNode* create_kheap_node(KHeapMemoryNode *, size_t);

size_t align(size_t);
uint64_t compute_kheap_end();
//Allocation functions
void *kmalloc(size_t);
void kfree(void *);

#endif
