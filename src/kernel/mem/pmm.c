#include <pmm.h>
#include <bitmap.h>
#include <stdbool.h>
#include <video.h>

extern uint32_t used_frames;
extern uint32_t number_of_entries;
extern uint32_t bitmap_size;

uint64_t pmm_alloc_frame(){
    _printStringAndNumber("number of elements used: ", used_frames);
    if( ! pmm_check_frame_availability() ) {
        return -1; // No more frames to allocate
    }
    return NULL;
}

bool pmm_check_frame_availability() {
    if(used_frames < bitmap_size){
        return true;
    }
    return false; 
}
