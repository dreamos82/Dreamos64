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
uint64_t _kernel_physical_end = 0x1190AC;
uint64_t kheap_size = 8 * PAGE_SIZE;

int main(){
    kernel_heap_start = NULL;
    kernel_heap_current_pos = NULL;
    kernel_heap_end = NULL;
    printf("KHeap Memory allocation tests\n");
    printf("=============================\n");
    printf("\t[test_kheap] (Init)\n");
    //void *kheap_start = malloc(8 * PAGE_SIZE);
    kernel_heap_start = malloc(8 * PAGE_SIZE);
    kernel_heap_start->is_free = true;
    printf("\t[test_kheap] (Init) Size allocated: %d\n", (8*PAGE_SIZE));
    kernel_heap_start->size = 8 * PAGE_SIZE;
    kernel_heap_start->next = NULL;
    kernel_heap_start->prev = NULL;
    printf("\t[test_kheap] (Init)  Initialized heap of size: %lu\n", kernel_heap_start->size);
    printf("\t[test_kheap] (Init) Address of kheap: 0x%X\n", kernel_heap_start);
    kernel_heap_end = kernel_heap_start;
    end_of_mapped_memory = (uint64_t) kernel_heap_end + 0x15000;
    printf("\t[test_kheap] (Init) Initializing sizeof heap structure: ... %lu\n", sizeof(KHeapMemoryNode));
    printf("\t[test_kheap] (Init) Address of kheap: 0x%X\n", kernel_heap_end);
    printf("\t[test_kheap] (Init) Kheap size: %d\n", get_kheap_size(kernel_heap_start));
    test_kmalloc();
    test_kfree();
    free(kernel_heap_start);
    return 0;
}


void test_kmalloc(){
    void *initial_end = (void *) kernel_heap_end;
    printf("Testing kmalloc\n");
    char *test_ptr = (char *) kmalloc(0);
    pretty_assert(NULL, test_ptr, ==, "Testing kmalloc(0) returns NULL");
    pretty_assert(kernel_heap_start, kernel_heap_end, ==, "Checking kernel_heap_tart == kernel_heap_end");
    test_ptr = NULL;
    pretty_assert(1, get_kheap_size(kernel_heap_start), ==, "Testing the size of the heap");
    test_ptr = (char *) kmalloc(10);
    pretty_assert(kernel_heap_start, (test_ptr - sizeof(KHeapMemoryNode)), ==, "kmalloc(10) Testing that kernel_heap_start == (test_ptr - sizeof(KHeapMemoryNode))");
    //assert(kernel_heap_start == (test_ptr - sizeof(KHeapMemoryNode)));
    pretty_assert((uint64_t) test_ptr, ((uint64_t)initial_end + sizeof(KHeapMemoryNode)), ==, "kmalloc(10) Testing the pointer returned value");
    assert((uint64_t)test_ptr == (((uint64_t)initial_end)  + sizeof(KHeapMemoryNode)));
    pretty_assert(kernel_heap_end->size, (kheap_size - (0x30 + sizeof(KHeapMemoryNode))), ==, "kmalloc(10) testing kheap size");
    printf("Finished\n");
}

void test_kfree(){
    printf("Test kfree\n");
    KHeapMemoryNode *original_end = kernel_heap_end;
    kfree(NULL);
    pretty_assert(original_end, kernel_heap_end, ==, "kfree(NULL) - kernel_original_end shouldn't have changed");
    char *test_ptr = (char *) kmalloc(10);
    uint8_t heap_length = get_kheap_size(kernel_heap_start);
    kfree(test_ptr);
    uint8_t new_heap_length = get_kheap_size(kernel_heap_start);
    pretty_assert(heap_length-1, new_heap_length, ==, "Testing kfree after kmalloc, checking heap_length");
    assert(new_heap_length == (heap_length -1));
    printf("Finished\n");
}
