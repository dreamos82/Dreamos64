#include <kheap.h>
#include <stdio.h>


KHeapMemoryNode *kernel_heap_start;
KHeapMemoryNode *kernel_heap_current_pos;
KHeapMemoryNode *kernel_heap_tail;
    
extern unsigned int _kernel_end;

void initialize_kheap(){
    //That should be mapped? 
    kernel_heap_start = (KHeapMemoryNode *) (&_kernel_end + KERNEL_MEMORY_PADDING);
    kernel_heap_current_pos = kernel_heap_start;
    kernel_heap_tail = kernel_heap_start;
    kernel_heap_current_pos->size = 0x1000;
    kernel_heap_current_pos->is_free = true;
    kernel_heap_current_pos->next = NULL;
    kernel_heap_current_pos->prev = NULL;
    printf("Initializing kernel memory allocator... %d\n", kernel_heap_current_pos->size);
}


void *kmalloc(size_t size){
    KHeapMemoryNode *current_node = kernel_heap_tail;
    if(size == 0){
        return NULL;
    }
    uint64_t header_size = sizeof(KHeapMemoryNode);
    if(((size + header_size) - current_node->size) > 1){
        current_node->is_free = false;
        current_node->next = NULL;
        KHeapMemoryNode *new_node = createKHeapNode(current_node, size);
        //KHeapMemoryNode *new_node = (KHeapMemoryNode *) (current_node + sizeof(KHeapMemoryNode) + size);
        new_node->is_free = true;
        new_node->size = current_node->size - (size + header_size);
        kernel_heap_tail = new_node;
        if(current_node->prev != NULL){
            KHeapMemoryNode *prev_node = current_node->prev;
        }
        return (void *) (current_node + sizeof(KHeapMemoryNode));
    }
    if(kernel_heap_start != kernel_heap_tail){
        if(kernel_heap_start->size > size + sizeof(KHeapMemoryNode)){
            kernel_heap_start->is_free = false;
            kernel_heap_start = kernel_heap_start->next;
        }
        current_node = kernel_heap_start->next;
        while(current_node->next != NULL){
            current_node = current_node->next;
            //Here i will search for a free node in the list (and split it in case)
            if(current_node->is_free){
                current_node->is_free = false;
                current_node->next = NULL;
                KHeapMemoryNode *new_node = (KHeapMemoryNode *) (current_node + sizeof(KHeapMemoryNode) + size);
                current_node->next = new_node;
                return (void *) current_node + sizeof(KHeapMemoryNode);
            }
    }


    }
    return NULL;
}

void kfree(void *ptr){
}

KHeapMemoryNode* createKHeapNode(KHeapMemoryNode *current_node, size_t size){
    uint64_t header_size = sizeof(KHeapMemoryNode);
    KHeapMemoryNode* new_node = (KHeapMemoryNode *) (current_node + sizeof(KHeapMemoryNode) + size);
    new_node->is_free = true;
    new_node->size = current_node->size - (size + header_size);
    new_node->prev = NULL;
    new_node->next = NULL;
    return new_node;

}
