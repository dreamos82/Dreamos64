#include <kheap.h>
#include <bitmap.h>
#include <vmm.h>
#include <kernel.h>
#include <logging.h>

KHeapMemoryNode *kernel_heap_start;
KHeapMemoryNode *kernel_heap_current_pos;
KHeapMemoryNode *kernel_heap_end;
    
extern unsigned int end_of_mapped_memory;

void initialize_kheap(){
    #ifndef _TEST_
    uint64_t *kheap_vaddress = vmm_alloc(PAGE_SIZE_IN_BYTES);
    uint64_t phys_address = pmm_alloc_frame();    
    map_phys_to_virt_addr((void*) phys_address, (void *)kheap_vaddress, PRESENT);
    kernel_heap_start = (KHeapMemoryNode *) ((uint64_t) kheap_vaddress);
    kernel_heap_start->size = 500;
    loglinef(Verbose, "(initialize_kheap) Start address using vmm_alloc: %x, and using end of vmm_space: %x", kheap_vaddress, kernel_heap_start);
    loglinef(Verbose, "(initialize_kheap) PAGESIZE: 0x%x - val: %x", PAGE_SIZE_IN_BYTES, kernel_heap_start->size);
    #else
    #pragma message "(initialize_kheap) Using test specific initialization"
    loglinef(Verbose, "(initialize_kheap) Test suite initialization");
    kernel_heap_start = (KHeapMemoryNode *) ((uint64_t)&_kernel_end + KERNEL_MEMORY_PADDING);
    #endif
    kernel_heap_current_pos = kernel_heap_start;
    kernel_heap_end = kernel_heap_start;
    //TODO: Should we use PAGE_SIZE for the initial heap size?
    kernel_heap_current_pos->size = 0x1000;
    kernel_heap_current_pos->is_free = true;
    kernel_heap_current_pos->next = NULL;
    kernel_heap_current_pos->prev = NULL;
    //loglinef(Verbose, "(initialize_kheap) Vmm (end_of_vmm_space): %x", end_of_vmm_space);
}

size_t align(size_t size) {
    return (size / HEAP_ALLOC_ALIGNMENT + 1) * HEAP_ALLOC_ALIGNMENT;
}

void *kmalloc(size_t size) {
    KHeapMemoryNode *current_node = kernel_heap_start;
    // If size is 0 we don't need to do anything 
    if( size == 0 ) {
        loglinef(Verbose, "(kmalloc) Size is null");
        return NULL;
    }

    //loglinef(Verbose, "(kmalloc) Current heap free size: 0x%x - Required: 0x%x", current_node->size, align(size + sizeof(KHeapMemoryNode))); 

    while( current_node != NULL ) {
        // The size of a node contains also the size of the header, so when creating nodes we add headers
        // We need to take it into account
        size_t real_size = size + sizeof(KHeapMemoryNode);
        //We also need to align it!
        real_size = align(real_size);
        if( current_node->is_free) {
            if( current_node->size >= real_size ) {
                // Ok we have found a node big enough
                if( current_node->size - real_size > KHEAP_MINIMUM_ALLOCABLE_SIZE ) {
                    // We can keep shrinking the heap, since we still have enough space!
                    // But we need a new node for the allocated area
                    create_kheap_node(current_node, real_size);
                    // Let's update current_node status
                    current_node->is_free = false;
                    current_node->size = real_size;
                } else {
                    // The current node space is not enough for shrinking, so we just need to mark the current_node as busy.
                    // Size should not be touched.
                    current_node->is_free = false;
                    //current_node->size -= real_size;
                }
                return (void *) current_node + sizeof(KHeapMemoryNode);
            }
        }

        if( current_node == kernel_heap_end ) {
            expand_heap(real_size);
            if( current_node->prev != NULL) {
                // If we are here it means that we were at the end of the heap and needed an expansion
                // So after the expansion there are chances that we reach the end of the heap, and the 
                // loop will end here. So let's move back of one item in the list, so we are sure the next item to be picked
                // will be the new one.
                current_node = current_node->prev;
            }
        }
        current_node = current_node->next;
    }
    return NULL;
}

void expand_heap(size_t required_size) {
    size_t number_of_pages = required_size / KERNEL_PAGE_SIZE + 1;
    uint64_t heap_end = compute_kheap_end();
    if( heap_end > end_of_mapped_memory ) {
        //end_of_mapped memory marks the end of the memory mapped by the kernel loader.
        //if the new heap address is above that, we need to map a new one, otherwise we can just mark it as used.
        //That part temporary, it needs to be reviewed when the memory mapping will be reviewed.
        map_vaddress_range((uint64_t *) heap_end, 0, number_of_pages);
    }
    KHeapMemoryNode *new_tail = (KHeapMemoryNode *) heap_end;
    new_tail->next = NULL;
    new_tail->prev = kernel_heap_end;
    new_tail->size = KERNEL_PAGE_SIZE * number_of_pages;
    new_tail->is_free = true;
    kernel_heap_end->next = new_tail;
    kernel_heap_end = new_tail;
    uint8_t available_merges = can_merge(new_tail);
    if ( available_merges & MERGE_LEFT) {
        merge_memory_nodes(new_tail->prev, new_tail);
    }
    
}

uint64_t compute_kheap_end() {
    return (uint64_t)kernel_heap_end + kernel_heap_end->size + sizeof(KHeapMemoryNode);
}


void kfree(void *ptr) {
    // Before doing anything let's check that the address provided is valid: not null, and within the heap space
    if(ptr == NULL) {
        return;
    }

    if ( (uint64_t) ptr < (uint64_t) kernel_heap_start || (uint64_t) ptr > (uint64_t) kernel_heap_end) {
        return;
    }

    // Now we can search for the node containing our address
    KHeapMemoryNode *current_node = kernel_heap_start;
    while( current_node != NULL ) {
        if( ((uint64_t) current_node + sizeof(KHeapMemoryNode)) == (uint64_t) ptr) {
            current_node->is_free = true;
            uint8_t available_merges = can_merge(current_node);
 
            if( available_merges & MERGE_RIGHT ) {
                merge_memory_nodes(current_node, current_node->next);
            }
           
            if( available_merges & MERGE_LEFT ) {
                merge_memory_nodes(current_node->prev, current_node);
            }
            return;
            
        }
        current_node = current_node->next;
    }
}

#ifdef _TEST_
uint8_t get_kheap_size(KHeapMemoryNode *heap_start) {
    KHeapMemoryNode *cur_node = heap_start;
    uint8_t size = 0;
    while( cur_node != NULL ) {
        size++;
        cur_node = cur_node->next;        
    }
    return size;
}
#endif

uint8_t can_merge(KHeapMemoryNode *cur_node) {
    // This function checks if the current node can be merged to both left and right
    // There return value is a 2 bits field: bit #0 is set if the node can be merged right
    // bit #1 is set if the node can be merged left. Bot bits set means it can merge in both diections
    KHeapMemoryNode *prev_node = cur_node->prev;
    KHeapMemoryNode *next_node = cur_node->next;
    uint8_t available_merges = 0;
    if( prev_node != NULL && prev_node->is_free ) {
        uint64_t prev_address = (uint64_t) prev_node + sizeof(KHeapMemoryNode) + prev_node->size;
        if ( prev_address == (uint64_t) cur_node ) {
            available_merges = available_merges | MERGE_LEFT;
        }
    }
    if( next_node != NULL && next_node->is_free ) {
        uint64_t next_address = (uint64_t) cur_node + sizeof(KHeapMemoryNode) + cur_node->size;
        if ( next_address == (uint64_t) cur_node->next ) {
            available_merges = available_merges | MERGE_RIGHT;
        }

    }

    return available_merges;
}

void merge_memory_nodes(KHeapMemoryNode *left_node, KHeapMemoryNode *right_node) {
    if(left_node == NULL || right_node == NULL) {
        return;
    }
    if(((uint64_t) left_node +  left_node->size + sizeof(KHeapMemoryNode)) == (uint64_t) right_node) {
        //We can combine the two nodes:
        //1. Sum the sizes
        left_node->size = left_node->size + right_node->size + sizeof(KHeapMemoryNode);
        //2. left_node next item will point to the next item of the right node (since the right node is going to disappear)
        left_node->next = right_node->next;
        //3. Unless we reached the last item, we should also make sure that the element after the right node, will be linked 
        //   to the left node (via the prev field)
        if(right_node->next != NULL){
            KHeapMemoryNode *next_node = right_node->next;
            next_node->prev = left_node;
        }
    } 
}


KHeapMemoryNode* create_kheap_node( KHeapMemoryNode *current_node, size_t size ) {
    // Here we create a new node for the heap.
    // We basically take the current node and split it in two.
    // The new node will be placed just after the space used by current_node
    // And current_node will be the node containing the information regarding the current kmalloc call.
    uint64_t header_size = sizeof(KHeapMemoryNode);
    KHeapMemoryNode* new_node = (KHeapMemoryNode *) ((void *)current_node + sizeof(KHeapMemoryNode) + size);
    new_node->is_free = true;
    new_node->size = current_node->size - (size + header_size);
    new_node->prev = current_node;
    new_node->next = current_node->next;
    
    if( current_node->next != NULL) {
        current_node->next->prev = new_node;
    }

    current_node->next = new_node;
    
    if( current_node == kernel_heap_end) {
        kernel_heap_end = new_node;
    }

    return new_node;
}
