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
extern  mmap_wrapper mmap_data;

void pmm_setup(){
    _initialize_bitmap();
    _setup_mmap();
}
void *pmm_alloc_frame(){
    //_printStringAndNumber("number of elements used: ", used_frames);
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

void pmm_free_frame(void *location){
    uint64_t frame = ((uint64_t)location) / PAGE_SIZE_IN_BYTES;
    _bitmap_free_bit(frame);
    used_frames--;
}

bool pmm_check_frame_availability() {
    if(used_frames < bitmap_size){
        return true;
    }
    return false; 
}

void pmm_reserve_area(uint64_t starting_address, uint32_t size){
    uint32_t counter = 0;
    while(counter < mmap_data.number_of_entries){
        counter++;
    }
    
    #ifndef _TEST_
    _printStringAndNumber("coming soon", counter);
    #endif

}
