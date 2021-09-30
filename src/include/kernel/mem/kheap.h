#ifdef __KHEAP__H
#define __KHEAP__H

typedef struct {
    void *address;
    uint64_t size;
    KHeapMemoryNode *next
} KHeapMemoryNode;

void initialize_kheap();

#endif
