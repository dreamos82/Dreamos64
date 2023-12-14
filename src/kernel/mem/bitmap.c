#include <bitmap.h>
#include <pmm.h>
#include <multiboot.h>
#include <stddef.h>
#include <mmap.h>
#include <logging.h>
#ifndef _TEST_
#include <video.h>
#include <main.h>
#include <vm.h>
#include <vmm.h>
#include <vmm_mapping.h>
#include <kernel.h>
#endif

#ifdef _TEST_
#include <stdlib.h>
#endif

extern struct multiboot_tag_basic_meminfo *tagmem;
extern uint64_t end_of_mapped_memory;
extern uint64_t _kernel_physical_end;
extern uint64_t _kernel_end;
extern uint64_t p4_table[];

size_t memory_size_in_bytes;
uint64_t *memory_map = (uint64_t *) &_kernel_end;
uint32_t number_of_entries = 0;
uint32_t bitmap_size = 0;
uint32_t used_frames;
uint64_t memory_map_phys_addr;


void _initialize_bitmap ( unsigned long end_of_reserved_area ) {
    uint64_t memory_size = (tagmem->mem_upper + 1024) * 1024;
    bitmap_size = memory_size / PAGE_SIZE_IN_BYTES + 1;
    used_frames = 0;
    number_of_entries = bitmap_size / 64 + 1;
    uint64_t memory_map_phys_addr;
#ifdef _TEST_
    memory_map = malloc(bitmap_size / 8 + 1);
#else
    memory_map_phys_addr = _mmap_determine_bitmap_region(end_of_reserved_area, bitmap_size / 8 + 1);
    //memory_map = memory_map_phys_addr;
    uint64_t end_of_mapped_physical_memory = end_of_mapped_memory - _HIGHER_HALF_KERNEL_MEM_START;
    if(memory_map_phys_addr > end_of_mapped_physical_memory) {
        pretty_logf(Verbose, "The address 0x%x is above the initially mapped memory: 0x%x", memory_map_phys_addr, end_of_mapped_physical_memory);
        //TODO: This need to be fixed map_phys_to_virt_addr can't be used here since it relies on the bitmap, and it is not initialized yet.
        map_phys_to_virt_addr((void*)ALIGN_PHYSADDRESS(memory_map_phys_addr), (void*)(memory_map_phys_addr + _HIGHER_HALF_KERNEL_MEM_START), VMM_FLAGS_PRESENT | VMM_FLAGS_WRITE_ENABLE);
    } else {
        pretty_logf(Verbose, "The address 0x%x is not above the initially mapped memory: 0x%x", memory_map_phys_addr, end_of_mapped_physical_memory);
    }
    memory_map = (uint64_t *) (memory_map_phys_addr + _HIGHER_HALF_KERNEL_MEM_START);

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
    used_frames = kernel_entries;
    pretty_logf(Info, "Page size used by the kernel: %d", PAGE_SIZE_IN_BYTES);
    pretty_logf(Verbose, "Physical size in bytes: %d", memory_size_in_bytes);
    pretty_logf(Verbose, "Number of bit entries: %d - %d", bitmap_size, number_of_entries);
}

void _bitmap_get_region(uint64_t* base_address, size_t* length_in_bytes, address_type_t type)
{
    if (type == ADDRESS_TYPE_PHYSICAL) {
        *base_address = (uint64_t)memory_map_phys_addr;
    } else if (type == ADDRESS_TYPE_VIRTUAL) {
        *base_address = (uint64_t)memory_map;
    }
    *length_in_bytes = bitmap_size / 8 + 1;
}

#ifndef _TEST_
uint32_t _compute_kernel_entries(uint64_t end_of_kernel_area){
    uint32_t kernel_entries = ((uint64_t)end_of_kernel_area) / PAGE_SIZE_IN_BYTES;
    uint32_t kernel_mod_entries = ((uint32_t)(end_of_kernel_area)) % PAGE_SIZE_IN_BYTES;
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
                    return row * BITMAP_ROW_BITS + column;
                }
            }
        }
    }
    return -1;
}

int64_t _bitmap_request_frames(size_t number_of_frames) {
    uint16_t row = 0;
    uint16_t column = 0;
    size_t adjacents_found = 0;
    uint16_t start_row =0;
    uint16_t start_column = 0;

    for (row = 0; row < number_of_entries; row++){
        if(memory_map[row] != BITMAP_ENTRY_FULL){
            for (column = 0; column < BITMAP_ROW_BITS; column++){
                uint64_t bit = 1 << column;
                if((memory_map[row] & bit) == 0){
                    if(adjacents_found == 0) {
                        start_row = row;
                        start_column = column;
                    }
                    adjacents_found++;
                    if(adjacents_found == number_of_frames) {
                        return start_row * BITMAP_ROW_BITS + start_column;
                    }
                } else {
                    adjacents_found = 0;
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


