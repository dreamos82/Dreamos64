#include <bitmap.h>
#include <pmm.h>
#include <multiboot.h>
#include <stddef.h>
#ifndef _TEST_
#include <video.h>
#include <main.h>
#endif
extern struct multiboot_tag_basic_meminfo *tagmem;
extern uint64_t _kernel_physical_end;
extern uint64_t _kernel_end;

uint64_t *memory_map = (uint64_t *) &_kernel_end;
uint32_t number_of_entries = 0;
uint32_t bitmap_size = 0;
uint32_t used_frames; 


void _initialize_bitmap(unsigned long addr){
    uint32_t mbi_size = *(uint32_t *) addr;
    _printStringAndNumber("Size of mbi struct: ", mbi_size);

    uint32_t memory_size_in_bytes = (tagmem->mem_upper + 1024) * 1024;
    bitmap_size = memory_size_in_bytes / PAGE_SIZE_IN_BYTES;
    used_frames = 0;
    _printStringAndNumber("KERNEL_END: 0x", &_kernel_end);
    _printStringAndNumber("KERNEL_END: 0x", &_kernel_physical_end);
    number_of_entries = bitmap_size / 64;
    for (uint32_t i=0; i<number_of_entries; i++){
        memory_map[i] = 0x0;
    }
    
    uint32_t kernel_entries = _compute_kernel_entries();
    uint32_t number_of_bitmap_rows = kernel_entries/64;
    uint32_t j=0;
    for (j=0; j < number_of_bitmap_rows; j++){
        memory_map[j] = ~(0);
    }
    memory_map[j] = ~(~(0l) << (kernel_entries - (number_of_bitmap_rows*64)));
    //used_frames = kernel_entries;
    //used_frames = 0x09; // Is the number of currently used frames - 1 (since the count starts from 0)
    used_frames = kernel_entries;
    _printStringAndNumber("Page size: ", PAGE_SIZE_IN_BYTES);
    _printStringAndNumber("Actual size in bytes: ", memory_size_in_bytes);
    _printStringAndNumber("Number of bit entries: ", bitmap_size);
    _printStringAndNumber("Number of items: ", number_of_entries);
    //_bitmap_request_frame();
    //pmm_alloc_frame();
}

#ifndef _TEST_
uint32_t _compute_kernel_entries(){
    uint32_t kernel_entries = ((uint64_t)&_kernel_physical_end) / PAGE_SIZE_IN_BYTES;
    uint32_t kernel_mod_entries = ((uint32_t)(&_kernel_physical_end)) % PAGE_SIZE_IN_BYTES;
    if (  kernel_mod_entries != 0){
        return kernel_entries + 2;
    } 
    return kernel_entries + 1;
   
}
#endif

/**
 * This function is returning the bitmap location of the first available page-frame
 *
 * */
int64_t _bitmap_request_frame(){
    uint16_t row = 0;
    uint16_t column = 0;
    for (row = 0; row < number_of_entries; row++){
        if(memory_map[row] != BITMAP_ENTRY_FULL){
            for (column = 0; column < BITMAP_ROW_BITS; column++){
                uint64_t bit = 1 << column;
                if((memory_map[row] & bit) == 0){
                    //Found a location
/*#if !defined(_TEST_) && defined(DEBUG)
                    _printStringAndNumber("Found something at row: ", row);
                    _printStringAndNumber("---column: ", column);
                    _printStringAndNumber("---Address: ", (row * 64 + column) * PAGE_SIZE_IN_BYTES); 
#endif*/
                    return row * BITMAP_ROW_BITS + column;
                }
            }
        }
    }
    return -1;
}



/**
 * In the next 3 function location is the bit-location inside the bitmap
 * */
void _bitmap_set_bit(uint64_t location){
    memory_map[location / BITMAP_ROW_BITS] |= 1 << (location % BITMAP_ROW_BITS);
}

void _bitmap_free_bit(uint64_t location){
    memory_map[location / BITMAP_ROW_BITS] &= ~(1 << (location % BITMAP_ROW_BITS));
}

bool _bitmap_test_bit(uint64_t location){
    return memory_map[location / BITMAP_ROW_BITS] & (1 << (location % BITMAP_ROW_BITS));
}


