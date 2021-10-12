#include <test_kheap.h>
#include <stdio.h>
#include <stdlib.h>
#include <kheap.h>
#include <assert.h>

extern KHeapMemoryNode* kernel_heap_start;
extern KHeapMemoryNode* kernel_heap_current_pos;
extern KHeapMemoryNode* kernel_heap_tail;

uint64_t _kernel_end = 0x1190AC;
uint64_t kheap_size = 8 * PAGE_SIZE;

int main(){
    kernel_heap_start = NULL;
    kernel_heap_current_pos = NULL;
    kernel_heap_tail = NULL;
    printf("To be implemented\n");
    //void *kheap_start = malloc(800);
    kernel_heap_start = (KHeapMemoryNode *)malloc(sizeof(KHeapMemoryNode));
    kernel_heap_start->is_free = true;
    printf("After malloc\n");
    kernel_heap_start->size = 8 * PAGE_SIZE;
    kernel_heap_start->next = NULL;
    kernel_heap_start->prev = NULL;
    printf("Initialized heap of size: %d\n", kernel_heap_start->size);
    printf("Address of kheap: 0x%X\n", kernel_heap_start);
    kernel_heap_tail = kernel_heap_start;
    printf("Initializing sizeof heap structure: ... %d\n", sizeof(KHeapMemoryNode));
    test_kmalloc();
    free(kernel_heap_start);
    return 0;
}


void test_kmalloc(){
    printf("Testing kmalloc with size 0\n");
    char *test_ptr = (char *) kmalloc(0);
    assert(test_ptr == NULL);
    printf("kheap size before kamlloc: 0x%X\n", kernel_heap_start->size);
    test_ptr = (char *) kmalloc(10);
    printf("Address of new location: 0x%X\n", test_ptr);
    printf("Address of new location: 0x%X\n", (kernel_heap_tail + sizeof(KHeapMemoryNode)));
    assert(test_ptr == (kernel_heap_start + sizeof(KHeapMemoryNode)));
    printf("kheap size after kamlloc: 0x%X\n", kernel_heap_tail->size);
    assert(kernel_heap_tail->size == (kheap_size - (10 + sizeof(KHeapMemoryNode))));
    
    //test_ptr = (char *) kmalloc(10);
    //assert(test_ptr == (kernel_heap_start + sizeof(KHeapMemoryNode)));
}
