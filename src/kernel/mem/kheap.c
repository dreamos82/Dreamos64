#include <kheap.h>
#include <stdio.h>
#include <bitmap.h>
#include <vmm.h>


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
    //We try to allocate from the list tail first.
    uint64_t header_size = sizeof(KHeapMemoryNode);
    size_t computed_size = (size + header_size) - current_node->size;
    while(current_node != NULL){
        KHeapMemoryNode *prev_node = current_node->prev;
        //Here i will search for a free node in the list (and split it in case)
        //probably worth adding some padding?
        if(current_node->is_free && current_node->size > size){
            if(current_node->size <= (size+header_size) ){
                //Need to remove the current node
                current_node->is_free = false;
                if(prev_node == NULL) {
                    //There is no prev node, so we are still on the top.
                    kernel_heap_tail = current_node->next;
                } else {
                    prev_node->next = current_node->next;
                }
                
            } else {
                KHeapMemoryNode *new_node = createKHeapNode(current_node, size);
                if(prev_node != NULL){
                    prev_node->next = new_node;
                } else {
                    kernel_heap_tail = new_node;
                }
            }
            return (void *) current_node + sizeof(KHeapMemoryNode);
        }
        current_node = current_node->next;
    }

    if (size > KERNEL_PAGE_SIZE) { 
        uint32_t number_of_pages = (size / KERNEL_PAGE_SIZE) + 1;
        printf("KMALLOC SIZE > PAGE_SIZE, partially unsupported for now\n");
        return  NULL;
    } else {
        uint64_t *new_address = ((void *)current_node) + current_node->size + sizeof(KHeapMemoryNode);
        map_vaddress(new_address, 0);
    }

    // TODO: allocate more physical space
    // Cases: size < page_size, size > page_size  
    return NULL;

}

void kfree(void *ptr){
}

KHeapMemoryNode* createKHeapNode(KHeapMemoryNode *current_node, size_t size){
    printf("Address: %x\n", current_node);
    uint64_t header_size = sizeof(KHeapMemoryNode);
    KHeapMemoryNode* new_node = (KHeapMemoryNode *) ((void *)current_node + sizeof(KHeapMemoryNode) + size);
    printf("New address: %x\n", (current_node + sizeof(KHeapMemoryNode) + size));
    new_node->is_free = true;
    new_node->size = current_node->size - (size + header_size);
    new_node->prev = current_node->prev;
    new_node->next = current_node->next;
    return new_node;

}
