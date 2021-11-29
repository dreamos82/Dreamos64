#include <pmm.h>
#include <bitmap.h>
#include <stdbool.h>
#include <mmap.h>
#ifndef _TEST_
#include <video.h>
#endif

extern uint32_t used_frames;
extern uint32_t number_of_entries;
extern uint32_t bitmap_size;
extern multiboot_memory_map_t *mmap_entries;
extern uint8_t count_physical_reserved;

void pmm_setup(unsigned long addr, uint32_t size){
    _initialize_bitmap(addr + size);
    uint64_t bitmap_start_addr;
    size_t bitmap_size;
    _bitmap_get_region(&bitmap_start_addr, &bitmap_size);
#ifndef _TEST_
    //we cant reserve the bitmap in testing scenarios, as malloc() can return any address, usually leading to a super high index when we try to reserve it.
    //this usually results in a seg fault as we try to access entries in the bitmap waaaay too large.
    //we could probably get around this hack by asking the host os to map the bitmap as it's expected address via it's vmm.
    pmm_reserve_area(bitmap_start_addr, bitmap_size);
#endif
    //_mmap_setup();
}

void *pmm_alloc_frame(){
    if( ! pmm_check_frame_availability() ) {
        return 0; // No more frames to allocate
    }

    uint64_t frame = _bitmap_request_frame();
    if (frame > 0) {
        _bitmap_set_bit(frame);
        used_frames++;
        return (void*)(frame * PAGE_SIZE_IN_BYTES);
    }
    return NULL;
}

void pmm_free_frame(void *address){
    uint64_t frame = ((uint64_t)address) / PAGE_SIZE_IN_BYTES;
    _bitmap_free_bit(frame);
    used_frames--;
}

bool pmm_check_frame_availability() {
    if(used_frames < bitmap_size){
        return true;
    }
    return false; 
}

void pmm_reserve_area(uint64_t starting_address, size_t size){
    uint64_t location = starting_address / PAGE_SIZE_IN_BYTES;
    uint32_t number_of_frames = size / PAGE_SIZE_IN_BYTES;
    if((size % PAGE_SIZE_IN_BYTES) != 0){
        number_of_frames++;
    }
    for(; number_of_frames > 0; number_of_frames--){
       if(!_bitmap_test_bit(location)){
           _bitmap_set_bit(location++);
           used_frames++;
       }
    }
}

void pmm_free_area(uint64_t starting_address, size_t size){
    uint64_t location = starting_address / PAGE_SIZE_IN_BYTES;
    uint32_t number_of_frames = size / PAGE_SIZE_IN_BYTES;
    //TODO Decide if the check should go to pmm_free_area instead of here 
    for(int i = 0; i < count_physical_reserved; i++){
        multiboot_uint64_t base_addr = mmap_entries[i].addr;
        multiboot_uint64_t len = mmap_entries[i].len;
        if(starting_address >= base_addr && starting_address < base_addr + len){
            return;
        }
    }
    if((size % PAGE_SIZE_IN_BYTES) != 0){
        number_of_frames++;
    }
    for(; number_of_frames > 0; number_of_frames--){
        _bitmap_free_bit(location);
        used_frames--;
    }
}
