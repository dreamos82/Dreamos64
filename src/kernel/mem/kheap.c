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
                //This case means we can't split the node more so it has to be removed from the list.
                //Need to remove the current node
                current_node->is_free = false;
                //Let's update the size too (it will make the free easier)
                current_node->size = size + sizeof(KHeapMemoryNode);
                if(prev_node == NULL) {
                    //There is no prev node, so we are still on the top.
                    kernel_heap_tail = current_node->next;
                } else {
                    //Here we are no longer on the top, so we need to update the previous node next-item
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
            printf("Found address at: 0x%x, size: 0x%x\n", current_node, current_node->size);
            current_node->size = size + sizeof(KHeapMemoryNode);
            current_node->is_free = false;
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

void kfree(void *ptr) {
    if((uint64_t) ptr == NULL) {
        return;
    }
    KHeapMemoryNode *ptr_header = (KHeapMemoryNode *) (ptr - sizeof(KHeapMemoryNode));
    printf("Size of KheapMemoryNode: 0x%x\n", sizeof(KHeapMemoryNode));
    printf("ptr_header size: 0x%x - is_free = 0x%x\n", ptr_header->size, ptr_header->is_free);
    KHeapMemoryNode *current_node = kernel_heap_tail;
    KHeapMemoryNode *prev_node = NULL;
    while(current_node != NULL) {
        // If the current_node address is > than ptr then 
        // the node containing ptr should be added before it
        // But we need to ensure that the list is kept sorted
        if((uint64_t) current_node > (uint64_t) ptr_header) {
            //I should add the ptr_header node; as prev of current_node.
            //This means i should add the current_node->prev as prev of ptr_header
            ptr_header->next = current_node;
            if(current_node->prev == NULL) {
                printf("Adding a new pointer on top of everything\n");
                //If current_node is the first node, than the new tail will be ptr_header
                kernel_heap_tail = ptr_header;
                //But also make sure that ptr_header->prev is NULL
                //This header is always present when memory is allocated
                ptr_header->prev = NULL;
            } else {
                ptr_header->prev = current_node->prev;
            }
            current_node->prev = ptr_header;
            //We can stop here no need to continue...
            return;
        }
        printf("Current_node->size: 0x%x - is_free: 0x%d\n", current_node->size, current_node->is_free);
        prev_node = current_node;
        current_node = current_node->next;
    }
    //If we are here it means that all all items in the free nodes list are before ptr_header
    //So we can place our ptr_header as the last item
    prev_node->next = ptr_header;
    ptr_header->prev = prev_node;
    ptr_header->is_free = true;
    /*if( *ptr!= NULL ) {
    }*/
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
