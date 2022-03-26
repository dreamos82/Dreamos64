#include <bitmap.h>
#include <pmm.h>
#include <multiboot.h>
#include <stddef.h>
#include <mmap.h>
#include <stdio.h>
#ifndef _TEST_
#include <video.h>
#include <main.h>
#endif

#ifdef _TEST_
#include <stdlib.h>
#endif

extern struct multiboot_tag_basic_meminfo *tagmem;
extern uint64_t _kernel_physical_end;
extern uint64_t _kernel_end;
extern size_t memory_size_in_bytes;

uint64_t *memory_map = (uint64_t *) &_kernel_end;
uint32_t number_of_entries = 0;
uint32_t bitmap_size = 0;
uint32_t used_frames; 


void _initialize_bitmap(unsigned long end_of_reserved_area){
    uint64_t memory_size = (tagmem->mem_upper + 1024) * 1024;
    bitmap_size = memory_size / PAGE_SIZE_IN_BYTES + 1;
    used_frames = 0;
    number_of_entries = bitmap_size / 64 + 1;
#ifdef _TEST_
    memory_map = malloc(bitmap_size / 8 + 1);
#else
    memory_map = _mmap_determine_bitmap_region(end_of_reserved_area, bitmap_size / 8 + 1);
#endif
    for (uint32_t i=0; i<number_of_entries; i++){
        memory_map[i] = 0x0;
    }
    
    uint32_t kernel_entries = _compute_kernel_entries(end_of_reserved_area);
    uint32_t number_of_bitmap_rows = kernel_entries/64;
    uint32_t j=0;
    for (j=0; j < number_of_bitmap_rows; j++){
        memory_map[j] = ~(0);
    }
    memory_map[j] = ~(~(0ul) << (kernel_entries - (number_of_bitmap_rows*64)));
    //used_frames = kernel_entries;
    //used_frames = 0x09; // Is the number of currently used frames - 1 (since the count starts from 0)
    used_frames = kernel_entries;
    printf("Page size: %d\n", PAGE_SIZE_IN_BYTES);
    printf("Actual size in bytes: %d\n", memory_size_in_bytes);
    printf("Number of bit entries: %d\n", bitmap_size);
    printf("Number of items: %d\n", number_of_entries);
    printf("Used frames: 0x%x\n", used_frames);
    //_bitmap_request_frame();
    //pmm_alloc_frame();
}

void _bitmap_get_region(uint64_t* base_address, size_t* length_in_bytes)
{
    *base_address = (uint64_t)memory_map;
    *length_in_bytes = bitmap_size / 8 + 1;
}

#ifndef _TEST_
uint32_t _compute_kernel_entries(uint64_t end_of_kernel_area){
    uint32_t kernel_entries = ((uint64_t)end_of_kernel_area) / PAGE_SIZE_IN_BYTES;
    uint32_t kernel_mod_entries = ((uint32_t)(end_of_kernel_area)) % PAGE_SIZE_IN_BYTES;
    printf("number of entries: 0x%x\n", kernel_entries);
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

void _bitmap_set_bit_from_address(uint64_t address) {
    if( address < memory_size_in_bytes ) {
        _bitmap_set_bit(ADDRESS_TO_BITMAP_ENTRY(address));
    }
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


