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
    printf("=== Memory allocation tests ===\n");
    printf("--Init ---\n");
    //void *kheap_start = malloc(8 * PAGE_SIZE);
    kernel_heap_start = malloc(8 * PAGE_SIZE);
    kernel_heap_start->is_free = true;
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
    printf("--- Tests ---\n"); 
    printf("Testing kmalloc with size 0\n");
    char *test_ptr = (char *) kmalloc(0);
    printf(" - kmalloc(0)  should return NULL\n");
    assert(test_ptr == NULL);
    assert(kernel_heap_start == kernel_heap_tail);
    test_ptr = (KHeapMemoryNode *) kmalloc(10);
    printf(" - address returne by kmalloc should be kheap_start + sizeof(KHeapMemoryNode)\n");
    //assert(kernel_heap_tail == (test_ptr - sizeof(KHeapMemoryNode)));
    assert(test_ptr == (((uint64_t)initial_tail)  + sizeof(KHeapMemoryNode)));
    printf("testing kheap size after kmlloc: 0x%X\n", kernel_heap_tail->size);
    assert(kernel_heap_tail->size == (kheap_size - (10 + sizeof(KHeapMemoryNode))));
    char *test_ptr_2 = (char *) kmalloc(20);
    printf(" - kmalloc new address should be heap_start + sizeof(KHeapMemoryNode) + 9\n");
    printf("  1: 0x%x - 2: 0x%x\n", test_ptr, test_ptr_2);
    assert(((uint64_t) test_ptr_2 - (uint64_t) test_ptr) == 0x2A);
}
