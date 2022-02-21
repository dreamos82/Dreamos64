#include <test_kheap.h>
#include <test_common.h>
#include <stdio.h>
#include <stdlib.h>
#include <kheap.h>
#include <assert.h>

extern KHeapMemoryNode* kernel_heap_start;
extern KHeapMemoryNode* kernel_heap_current_pos;
extern KHeapMemoryNode* kernel_heap_end;

uint64_t _kernel_end = 0x1190AC;
uint64_t kheap_size = 8 * PAGE_SIZE;

int main(){
    kernel_heap_start = NULL;
    kernel_heap_current_pos = NULL;
    kernel_heap_end = NULL;
    printf("=== Memory allocation tests ===\n");
    printf("--- Init ---\n");
    //void *kheap_start = malloc(8 * PAGE_SIZE);
    kernel_heap_start = malloc(8 * PAGE_SIZE);
    kernel_heap_start->is_free = true;
    printf("* Size allocated: %d\n", (8*PAGE_SIZE));
    kernel_heap_start->size = 8 * PAGE_SIZE;
    kernel_heap_start->next = NULL;
    kernel_heap_start->prev = NULL;
    printf("*  Initialized heap of size: %d\n", kernel_heap_start->size);
    printf("Address of kheap: 0x%X\n", kernel_heap_start);
    kernel_heap_end = kernel_heap_start;
    printf("* Initializing sizeof heap structure: ... %d\n", sizeof(KHeapMemoryNode));
    printf("* Address of kheap: 0x%X\n", kernel_heap_end);
    test_kmalloc();
    test_kfree();
    free(kernel_heap_start);
    return 0;
}


void test_kmalloc(){
    void *initial_end = (void *) kernel_heap_end;
    printf("--- Testing kmalloc ---\n"); 
    printf("- Testing kmalloc with size 0\n");
    char *test_ptr = (char *) kmalloc(0);
    printf(" - kmalloc(0)  should return NULL\n");
    assert(test_ptr == NULL);
    assert(kernel_heap_start == kernel_heap_end);
    test_ptr = (char *) kmalloc(10);
/*        //assert(kernel_heap_end == (test_ptr - sizeof(KHeapMemoryNode)));
    assert((uint64_t)test_ptr == (((uint64_t)initial_end)  + sizeof(KHeapMemoryNode)));
    printf("testing kheap size after kmlloc: 0x%X\n", kernel_heap_end->size);
    assert(kernel_heap_end->size == (kheap_size - (10 + sizeof(KHeapMemoryNode))));
    char *test_ptr_2 = (char *) kmalloc(20);
    printf(" - kmalloc new address should be heap_start + sizeof(KHeapMemoryNode) + 9\n");
    printf("  1: 0x%x - 2: 0x%x\n", test_ptr, test_ptr_2);
    assert(((uint64_t) test_ptr_2 - (uint64_t) test_ptr) == 0x2A);
    printf("Finished\n");*/
}

void test_kfree(){
    printf("--- Testing kfree ---\n");
    KHeapMemoryNode *original_end = kernel_heap_end;
    printf("- Testing kfree with null, nothing should happen\n");
    kfree(NULL);
    assert(original_end == kernel_heap_end);
    /*printf("- Testing kfree right after a malloc\n");
    char *test_ptr = (char *) kmalloc(10);
    kfree(test_ptr);
    printf("- Testing kernel_heap_end:0x%x==(test_ptr-sizeof(KHeamMemoryNode)):0x%x\n", kernel_heap_end, (test_ptr - sizeof(KHeapMemoryNode)));
    assert((uint64_t) kernel_heap_end == (uint64_t) (test_ptr - sizeof(KHeapMemoryNode)));
    printf("- Testing two consecutive mallocs\n");
    char *test_ptr2 = (char *) kmalloc(10);
    printf("test_ptr2: 0x%x\n", test_ptr2);
    char *test_ptr3 = (char *) kmalloc(20);
    printf("test_ptr3: 0x%x\n", test_ptr3);
    kfree(test_ptr2);
    kfree(test_ptr3);
    printf("Finished\n");*/
}
