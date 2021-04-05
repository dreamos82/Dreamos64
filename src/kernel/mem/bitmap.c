#include <bitmap.h>
#include <multiboot.h>
#include <main.h>
#include <video.h>
#include <stddef.h>

extern struct multiboot_tag_basic_meminfo *tagmem;
extern unsigned long _kernel_physical_end;

uint64_t *memory_map;
uint32_t number_of_entries = 0;
void _initialize_bitmap(){
    //TODO
    //1. get upper memory size
    //2. make sure everything is zero
    //3. Compute number of entries needed to keep track of the physical ram
    //4. mark kernel area as reserved
    //5. Mark other addresses that are below UPPER_MEMORY size and are in the memory map as reserved.
    uint32_t memory_size_in_bytes = (tagmem->mem_upper + 1024) * 1024;
    uint32_t bitmap_size = memory_size_in_bytes / PAGE_SIZE_IN_BYTES;
    number_of_entries = bitmap_size / 64;
    for (int i=0; i<number_of_entries; i++){
        memory_map[i] = 0x0;
    }
    memory_map[0] = 0x3ff; //I'm marking first 10 element in the bitmap as taken
    _printStringAndNumber("Actual size in bytes: ", memory_size_in_bytes);
    _printStringAndNumber("Number of bit entries: ", bitmap_size);
    _printStringAndNumber("Number items: ", number_of_entries);
    _bitmap_request_frames(2);
}

uint64_t _bitmap_request_frames(uint8_t number_of_frames){
    uint16_t row = 0;
    uint16_t column = 0;
    for (row = 0; row < number_of_entries; row++){
        if(memory_map[row] != BITMAP_ENTRY_FULL){
            for (column = 0; column < BITMAP_ROW_BITS; column++){
                uint64_t bit = 1 << column;
                if((memory_map[row] & bit) == 0){
                    //Found a location
                    _printStringAndNumber("Found something at row: ", row);
                    _printStringAndNumber("---column: ", column);
                    return 1;
                }
            }
        }
    }
    return NULL;
}
