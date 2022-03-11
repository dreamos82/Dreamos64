#include <test_kheap.h>
#include <test_common.h>
#include <stdio.h>
#include <stdlib.h>
#include <kheap.h>
#include <assert.h>

extern KHeapMemoryNode* kernel_heap_start;
extern KHeapMemoryNode* kernel_heap_current_pos;
extern KHeapMemoryNode* kernel_heap_end;

unsigned int end_of_mapped_memory;;

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
    end_of_mapped_memory = kernel_heap_end + 0x15000;
    printf("* Initializing sizeof heap structure: ... %d\n", sizeof(KHeapMemoryNode));
    printf("* Address of kheap: 0x%X\n", kernel_heap_end);
    printf("- Kheap size: %d\n", get_kheap_size(kernel_heap_start));
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
    test_ptr = NULL;
    printf(" - Testing that the size of the heap is 1\n");
    assert(get_kheap_size(kernel_heap_start) == 1);
    test_ptr = (char *) kmalloc(10);
    printf("test_ptr value: 0x%x\n", test_ptr);
    assert(kernel_heap_start == (test_ptr - sizeof(KHeapMemoryNode)));
    assert((uint64_t)test_ptr == (((uint64_t)initial_end)  + sizeof(KHeapMemoryNode)));
    printf("testing kheap size after kmalloc: 0x%X\n", kernel_heap_end->size);
    assert(kernel_heap_end->size == (kheap_size - (0x30 + sizeof(KHeapMemoryNode))));
}

void test_kfree(){
    printf("--- Testing kfree ---\n");
    KHeapMemoryNode *original_end = kernel_heap_end;
    printf("Kernel heap end size: 0x%x\n", kernel_heap_end->size);
    printf("KHeapMemoryNode size: 0x%x\n", sizeof(KHeapMemoryNode));
    kfree(NULL);
    assert(original_end == kernel_heap_end);
    printf("- Testing kfree right after a malloc\n");
    printf("- Kheap size: %d\n", get_kheap_size(kernel_heap_start));
    char *test_ptr = (char *) kmalloc(10);
    uint8_t heap_length = get_kheap_size(kernel_heap_start);
    printf("- Kheap size: %d\n", heap_length);
    printf("Kernel heap end size: 0x%x\n", kernel_heap_end->size);
    printf("test_ptr: 0x%x\n", test_ptr);
    kfree(test_ptr);
    uint8_t new_heap_length = get_kheap_size(kernel_heap_start);
    printf("- Testing Kheap size: %d should be equal to: %d-1\n", new_heap_length, heap_length);
    assert(new_heap_length == (heap_length -1));
}
