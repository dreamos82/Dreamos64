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

#define MERGE_RIGHT 0b01
#define MERGE_LEFT  0b10
#define MERGE_BOTH  0b11
#define MERGE_NONE  0b00

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
uint8_t can_merge( KHeapMemoryNode *);
void expand_heap(size_t);

#ifdef _TEST_
uint8_t get_kheap_size(KHeapMemoryNode *);
#endif

#endif
