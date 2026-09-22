#include "include/test_kheap.h"
#include "include/test_stats.h"
#include <test_kheap.h>
#include <test_common.h>
#include <test_stats.h>
#include <stdio.h>
#include <stdlib.h>
#include <kheap.h>
#include <unistd.h>



test_runner_t *tests;
bool has_pipe = false;
unsigned int number_of_tests = 0;

extern KHeapMemoryNode* kernel_heap_start;
extern KHeapMemoryNode* kernel_heap_current_pos;
extern KHeapMemoryNode* kernel_heap_end;

unsigned int end_of_mapped_memory;
struct multiboot_tag_basic_meminfo *tagmem;
struct multiboot_tag_mmap *mmap_root;

uint64_t _kernel_end = 0x1190AC;
uint64_t _kernel_physical_end = 0x1190AC;
uint64_t kheap_size = 8 * PAGE_SIZE;

int main(int argc, char **argv){
    int pipe_fd = -1;
    if (argc > 1) {
            //pipe_fd = atoi(argv[1]);
            has_pipe = true;
            pipe_fd = atoi(argv[1]);
            printf("Found pipe id: %d\n", pipe_fd);
    }
    KHeapMemoryNode* kernel_heap_start_root = kernel_heap_start;
    kernel_heap_start = NULL;
    kernel_heap_current_pos = NULL;
    kernel_heap_end = NULL;
    printf("KHeap Memory allocation tests\n");
    printf("=============================\n");
    printf("[test_kheap] (Init)\n");
    //void *kheap_start = malloc(8 * PAGE_SIZE);
    kernel_heap_start = malloc(8 * PAGE_SIZE);
    kernel_heap_start->is_free = true;
    printf("[test_kheap] (Init) Size allocated: %d\n", (8*PAGE_SIZE));
    kernel_heap_start->size = 8 * PAGE_SIZE;
    kernel_heap_start->next = NULL;
    kernel_heap_start->prev = NULL;
    printf("[test_kheap] (Init)  Initialized heap of size: %lu\n", kernel_heap_start->size);
    printf("[test_kheap] (Init) Address of kheap: 0x%X\n", kernel_heap_start);
    kernel_heap_end = kernel_heap_start;
    end_of_mapped_memory = (uint64_t) kernel_heap_end + 0x15000;
    printf("[test_kheap] (Init) Initializing sizeof heap structure: ... %lu\n", sizeof(KHeapMemoryNode));
    printf("[test_kheap] (Init) Address of kheap: 0x%X\n", kernel_heap_end);
    printf("[test_kheap] (Init) Kheap size: %d\n", get_kheap_size(kernel_heap_start));
    prepare_tests();
    for (int i = 0; i<number_of_tests; i++){
        printf("%d) %s:\n", tests[i].stats.module_id, tests[i].stats.module_title);
        tests[i].handler(i);
        print_stats(tests[i].stats);
    }
    free(kernel_heap_start_root);
    if (has_pipe){
        write(pipe_fd, &number_of_tests, sizeof(number_of_tests));
        for(int j=0; j < number_of_tests; j++) {
            send_stats(pipe_fd, tests[j].stats);
        }
        close(pipe_fd);
    }
    printf("Finished\n");
    return 0;
}

void prepare_tests(){
    test_init(1, &tests);
    add_test(1, "Testing kmalloc", test_kmalloc, tests);
    number_of_tests++;
    add_test(2, "Test kfree", test_kfree, tests);
    number_of_tests++;
}


void test_kmalloc(unsigned int id){
    void *initial_end = (void *) kernel_heap_end;    
    char *test_ptr = (char *) kmalloc(0);
    pretty_assert_stat(NULL, test_ptr, ==, tests[id].stats, "Testing kmalloc(0) returns NULL", has_pipe);
    pretty_assert_stat(kernel_heap_start, kernel_heap_end, ==, tests[id].stats, "Checking kernel_heap_tart == kernel_heap_end", has_pipe);
    test_ptr = NULL;
    pretty_assert_stat(1, get_kheap_size(kernel_heap_start), ==, tests[id].stats, "Testing the size of the heap", has_pipe);
    test_ptr = (char *) kmalloc(10);
    pretty_assert_stat(kernel_heap_start, (test_ptr - sizeof(KHeapMemoryNode)), ==, tests[id].stats, "kmalloc(10) Testing that kernel_heap_start == (test_ptr - sizeof(KHeapMemoryNode))", has_pipe);
    //assert(kernel_heap_start == (test_ptr - sizeof(KHeapMemoryNode)));
    pretty_assert_stat((uint64_t) test_ptr, ((uint64_t)initial_end + sizeof(KHeapMemoryNode)), ==, tests[id].stats, "kmalloc(10) Testing the pointer returned value", has_pipe);
    pretty_assert_stat(kernel_heap_end->size, (kheap_size - (0x30 + sizeof(KHeapMemoryNode))), ==, tests[id].stats, "kmalloc(10) testing kheap size", has_pipe);
}

void test_kfree(unsigned int id){
    KHeapMemoryNode *original_end = kernel_heap_end;
    kfree(NULL);
    pretty_assert_stat(original_end, kernel_heap_end, ==, tests[id].stats, "kfree(NULL) - kernel_original_end shouldn't have changed", has_pipe);
    char *test_ptr = (char *) kmalloc(10);
    uint8_t heap_length = get_kheap_size(kernel_heap_start);
    kfree(test_ptr);
    uint8_t new_heap_length = get_kheap_size(kernel_heap_start);
    pretty_assert_stat(heap_length-1, new_heap_length, ==, tests[id].stats, "Testing kfree after kmalloc, checking heap_length", has_pipe);
}
