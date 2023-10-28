#include <test_kheap.h>
#include <test_common.h>
#include <stdio.h>
#include <stdlib.h>
#include <kheap.h>
#include <assert.h>

extern KHeapMemoryNode* kernel_heap_start;
extern KHeapMemoryNode* kernel_heap_current_pos;
extern KHeapMemoryNode* kernel_heap_end;

unsigned int end_of_mapped_memory;
struct multiboot_tag_basic_meminfo *tagmem;
struct multiboot_tag_mmap *mmap_root;

uint64_t _kernel_end = 0x1190AC;
uint64_t kheap_size = 8 * PAGE_SIZE;

int main(){
    kernel_heap_start = NULL;
    kernel_heap_current_pos = NULL;
    kernel_heap_end = NULL;
    printf("KHeap Memory allocation tests\n");
    printf("=============================\n");
    printf("\t [test_kheap] (Init)\n");
    //void *kheap_start = malloc(8 * PAGE_SIZE);
    kernel_heap_start = malloc(8 * PAGE_SIZE);
    kernel_heap_start->is_free = true;
    printf("\t [test_kheap] (Init) Size allocated: %d\n", (8*PAGE_SIZE));
    kernel_heap_start->size = 8 * PAGE_SIZE;
    kernel_heap_start->next = NULL;
    kernel_heap_start->prev = NULL;
    printf("\t [test_kheap] (Init)  Initialized heap of size: %d\n", kernel_heap_start->size);
    printf("\t [test_kheap] (Init) Address of kheap: 0x%X\n", kernel_heap_start);
    kernel_heap_end = kernel_heap_start;
    end_of_mapped_memory = (uint64_t) kernel_heap_end + 0x15000;
    printf("\t [test_kheap] (Init) Initializing sizeof heap structure: ... %d\n", sizeof(KHeapMemoryNode));
    printf("\t [test_kheap] (Init) Address of kheap: 0x%X\n", kernel_heap_end);
    printf("\t [test_kheap] (Init) Kheap size: %d\n", get_kheap_size(kernel_heap_start));
    test_kmalloc();
    test_kfree();
    free(kernel_heap_start);
    return 0;
}


void test_kmalloc(){
    void *initial_end = (void *) kernel_heap_end;
    printf("Testing kmalloc\n");
    printf("\t [test_kheap] (kmalloc) Testing kmalloc with size 0\n");
    char *test_ptr = (char *) kmalloc(0);
    printf("\t [test_kheap] (kmalloc) kmalloc(0)  should return NULL\n");
    assert(test_ptr == NULL);
    assert(kernel_heap_start == kernel_heap_end);
    test_ptr = NULL;
    printf("\t [test_kheap] (kmalloc) Testing that the size of the heap is 1\n");
    assert(get_kheap_size(kernel_heap_start) == 1);
    test_ptr = (char *) kmalloc(10);
    printf("\t [test_kheap] (kmalloc) test_ptr value: 0x%x\n", test_ptr);
    assert(kernel_heap_start == (test_ptr - sizeof(KHeapMemoryNode)));
    assert((uint64_t)test_ptr == (((uint64_t)initial_end)  + sizeof(KHeapMemoryNode)));
    printf("\t [test_kheap] (kmalloc) testing kheap size after kmalloc: 0x%X\n", kernel_heap_end->size);
    assert(kernel_heap_end->size == (kheap_size - (0x30 + sizeof(KHeapMemoryNode))));
    printf("Finished\n");
}

void test_kfree(){
    printf("Test kfree\n");
    KHeapMemoryNode *original_end = kernel_heap_end;
    printf("\t [test_kheap] (kfree) Kernel heap end size: 0x%x\n", kernel_heap_end->size);
    printf("\t [test_kheap] (kfree) KHeapMemoryNode size: 0x%x\n", sizeof(KHeapMemoryNode));
    kfree(NULL);
    assert(original_end == kernel_heap_end);
    printf("\t [test_kheap] (kfree) Testing kfree right after a malloc\n");
    printf("\t [test_kheap] (kfree) Kheap size: %d\n", get_kheap_size(kernel_heap_start));
    char *test_ptr = (char *) kmalloc(10);
    uint8_t heap_length = get_kheap_size(kernel_heap_start);
    printf("\t [test_kheap] (kfree) Kheap size: %d\n", heap_length);
    printf("\t [test_kheap] (kfree) Kernel heap end size: 0x%x\n", kernel_heap_end->size);
    printf("\t [test_kheap] (kfree) test_ptr: 0x%x\n", test_ptr);
    kfree(test_ptr);
    uint8_t new_heap_length = get_kheap_size(kernel_heap_start);
    printf("\t [test_kheap] (kfree) Testing Kheap size: %d should be equal to: %d-1\n", new_heap_length, heap_length);
    assert(new_heap_length == (heap_length -1));
    printf("Finished\n");
}
