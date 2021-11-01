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

typedef struct KHeapMemoryNode {
    uint64_t size;
    bool is_free;
    struct KHeapMemoryNode *next;
    struct KHeapMemoryNode *prev;
} KHeapMemoryNode;

//Service functions
void initialize_kheap();
KHeapMemoryNode* createKHeapNode(KHeapMemoryNode *, size_t);

//Allocation functions
void *kmalloc(size_t);
void kfree(void *);

#endif
