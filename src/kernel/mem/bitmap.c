#include <bitmap.h>
#include <pmm.h>
#include <multiboot.h>
#include <stddef.h>
#ifndef _TEST_
#include <video.h>
#include <main.h>
#endif
extern struct multiboot_tag_basic_meminfo *tagmem;
extern unsigned long _kernel_physical_end;

uint64_t *memory_map;
uint32_t number_of_entries = 0;
uint32_t bitmap_size = 0;
uint32_t used_frames; 


void _initialize_bitmap(){
    //TODO
    //1. get upper memory size
    //2. make sure everything is zero
    //3. Compute number of entries needed to keep track of the physical ram
    //4. mark kernel area as reserved
    //5. Mark other addresses that are below UPPER_MEMORY size and are in the memory map as reserved.
    uint32_t memory_size_in_bytes = (tagmem->mem_upper + 1024) * 1024;
    bitmap_size = memory_size_in_bytes / PAGE_SIZE_IN_BYTES;
    used_frames = 0;
    number_of_entries = bitmap_size / 64;
    for (int i=0; i<number_of_entries; i++){
        memory_map[i] = 0x0;
    }
    memory_map[0] = 0x3ff; //I'm marking first 10 element in the bitmap as taken
    used_frames = 0x09; // Is the number of currently used frames - 1 (since the count starts from 0)
#ifndef _TEST_
    _printStringAndNumber("Page size: ", PAGE_SIZE_IN_BYTES);
    _printStringAndNumber("Actual size in bytes: ", memory_size_in_bytes);
    _printStringAndNumber("Number of bit entries: ", bitmap_size);
    _printStringAndNumber("Number items: ", number_of_entries);
#endif
    _bitmap_request_frame();
    //pmm_alloc_frame();
}

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
#ifndef _TEST_
                    _printStringAndNumber("Found something at row: ", row);
                    _printStringAndNumber("---column: ", column);
                    _printStringAndNumber("---Address: ", (row * 64 + column) * PAGE_SIZE_IN_BYTES); 
#endif
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


