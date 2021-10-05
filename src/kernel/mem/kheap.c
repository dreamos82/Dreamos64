#include <kheap.h>
#include <stdio.h>
#include <stddef.h>


KHeapMemoryNode *kernel_heap_start;
KHeapMemoryNode *kernel_heap_current_pos;
    
extern unsigned int _kernel_end;

void initialize_kheap(){
    //That should be mapped? 
    kernel_heap_start = (KHeapMemoryNode *) (&_kernel_end + KERNEL_MEMORY_PADDING);
    kernel_heap_current_pos = kernel_heap_start;
    kernel_heap_current_pos->size = 0x1000;
    kernel_heap_current_pos->is_free = true;
    kernel_heap_current_pos->next = NULL;
    kernel_heap_current_pos->prev = NULL;
    printf("Initializing kernel memory allocator... %d\n", kernel_heap_current_pos->size);
}


void *kmalloc(size_t size){
    KHeapMemoryNode *current_node = kernel_heap_start;
    if(current_node->is_free){
        current_node->is_free = false;
        KHeapMemoryNode *new_node = (KHeapMemoryNode *) (current_node + sizeof(KHeapMemoryNode) + size);
        current_node->next = new_node;
        return (void *) current_node + sizeof(KHeapMemoryNode);
    }
    return NULL;
}

void kfree(void *ptr){
}
