#include <test_kheap.h>
#include <test_common.h>
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
    //void *kheap_start = malloc(8 * PAGE_SIZE);
    kernel_heap_start = malloc(8 * PAGE_SIZE);
    kernel_heap_start->is_free = true;
    printf("After malloc\n");
    printf("Size allocated: %d\n", (8*PAGE_SIZE));
    kernel_heap_start->size = 8 * PAGE_SIZE;
    kernel_heap_start->next = NULL;
    kernel_heap_start->prev = NULL;
    printf("Initialized heap of size: %d\n", kernel_heap_start->size);
    printf("Address of kheap: 0x%X\n", kernel_heap_start);
    kernel_heap_tail = kernel_heap_start;
    printf("Initializing sizeof heap structure: ... %d\n", sizeof(KHeapMemoryNode));
    printf("Address of kheap: 0x%X\n", kernel_heap_tail);
    test_kmalloc();
    free(kernel_heap_start);
    return 0;
}


void test_kmalloc(){
    void *initial_tail = (void *) kernel_heap_tail;
    printf("Testing kmalloc with size 0\n");
    char *test_ptr = (char *) kmalloc(0);
    assert(test_ptr == NULL);
    printf("kheap size before kmalloc: 0x%X\n", kernel_heap_start->size);
    printf("Addres of heap start and heap end should be equals\n");
    assert(kernel_heap_start == kernel_heap_tail);
    test_ptr = (KHeapMemoryNode *) kmalloc(10);
    printf("kernel_heap_tail Address of new location: 0x%X\n", kernel_heap_tail);
    printf("test_ptr - sizeof(node_header) : Address of new location: 0x%X\n", test_ptr - sizeof(KHeapMemoryNode));
    printf("size of header: 0x%X\n", sizeof(KHeapMemoryNode));  
    printf("kernel_heap_tail + sizeof(node_header) : Address of new location: 0x%X\n", (((uint64_t)initial_tail) + sizeof(KHeapMemoryNode)));
    //assert(kernel_heap_tail == (test_ptr - sizeof(KHeapMemoryNode)));
    assert(test_ptr == (((uint64_t)initial_tail)  + sizeof(KHeapMemoryNode)));
    printf("kheap size after kamlloc: 0x%X\n", kernel_heap_tail->size);
    assert(kernel_heap_tail->size == (kheap_size - (10 + sizeof(KHeapMemoryNode))));
    char *test_ptr_2 = (char *) kmalloc(20);
    printf("kmalloc new address should be heap_start + sizeof(KHeapMemoryNode) + 10\n");
    printf("1: 0x%x - 2: 0x%x\n", test_ptr, test_ptr_2);
    assert(((uint64_t) test_ptr_2 - (uint64_t) test_ptr) == 0x2A);
    //test_ptr = (char *) kmalloc(10);
}
