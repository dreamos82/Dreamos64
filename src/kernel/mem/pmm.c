#include <pmm.h>
#include <bitmap.h>
#include <stdbool.h>
#include <mmap.h>
#include <vmm.h>
#include <logging.h>
#include <spinlock.h>

#ifndef _TEST_
#include <video.h>
#endif

extern uint32_t used_frames;
extern uint32_t number_of_entries;
extern uint32_t bitmap_size;
extern multiboot_memory_map_t *mmap_entries;
extern uint8_t count_physical_reserved;
extern size_t memory_size_in_bytes;

spinlock_t memory_spinlock;

void pmm_setup(unsigned long addr, uint32_t size){
    _initialize_bitmap(addr + size);
    uint64_t bitmap_start_addr;
    size_t bitmap_size;
    _bitmap_get_region(&bitmap_start_addr, &bitmap_size, ADDRESS_TYPE_PHYSICAL);
    spinlock_release(&memory_spinlock);
#ifndef _TEST_
    //we cant reserve the bitmap in testing scenarios, as malloc() can return any address, usually leading to a super high index when we try to reserve it.
    //this usually results in a seg fault as we try to access entries in the bitmap waaaay too large.
    //we could probably get around this hack by asking the host os to map the bitmap as it's expected address via it's vmm. 
    pmm_reserve_area(bitmap_start_addr, bitmap_size);
#endif

    _map_pmm();
}

void _map_pmm()
{
    //late init after vmm has been initialized, we can do all sorts of wizardry now.
#ifdef _TEST_
    #pragma message "map_pmm() does nothing in testing scenarios, see notes about hack in pmm.c::pmm_setup()"
    return;
#endif
    uint64_t bitmap_start;
    size_t bitmap_size_bytes;
    _bitmap_get_region(&bitmap_start, &bitmap_size_bytes, ADDRESS_TYPE_PHYSICAL);
    //now we have the real addresses, we need to round the start down, and the size up to the nearest page    
    bitmap_start = (bitmap_start / PAGE_SIZE_IN_BYTES) * PAGE_SIZE_IN_BYTES;
    
    const size_t pages_required = bitmap_size_bytes / PAGE_SIZE_IN_BYTES + 1;

    loglinef(Verbose, "(_map_pmm): Identity mapping PMM bitmap, addr(virt & phys)= 0x%x", bitmap_start);
    loglinef(Verbose, "(_map_pmm):   \\- Pages required=%d", pages_required);

    for (size_t i = 0; i < pages_required; i++)
        map_vaddress((void*)(bitmap_start + i * PAGE_SIZE_IN_BYTES), 0); //0 as no extra flags required

    loglinef(Verbose, "(_map_pmm): PMM bitmap successfully mapped.");
}

/**
 * This function allocate a physical frame of memory.
 * 
 * @return The physical address of the allocated frame of memory of PAGE_SIZE_IN_BYTES
 */
void *pmm_alloc_frame(){
    if( ! pmm_check_frame_availability() ) {
        return 0; // No more frames to allocate
    }

    spinlock_acquire(&memory_spinlock);
    uint64_t frame = _bitmap_request_frame();
    if (frame > 0) {
        _bitmap_set_bit(frame);
        used_frames++;
        spinlock_release(&memory_spinlock);
        return (void*)(frame * PAGE_SIZE_IN_BYTES);
    }
    spinlock_release(&memory_spinlock);
    return NULL;
}

void *pmm_alloc_area(size_t size) {
    size_t requested_frames = get_number_of_pages_from_size(size);

    loglinef(Verbose, "(pmm_alloc_area): requested_frames: %x\n", requested_frames);
    spinlock_acquire(&memory_spinlock);
    uint64_t frames = _bitmap_request_frames(requested_frames);
    
    for (int i =0; i < requested_frames; i++) {
        _bitmap_set_bit( frames + i );
    }
    
    used_frames += requested_frames;
    spinlock_release(&memory_spinlock);
    return (void *) frames;
}

void pmm_free_frame(void *address){
    spinlock_acquire(&memory_spinlock);
    uint64_t frame = ((uint64_t)address) / PAGE_SIZE_IN_BYTES;
    _bitmap_free_bit(frame);
    spinlock_release(&memory_spinlock);
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
    spinlock_acquire(&memory_spinlock);
    for(; number_of_frames > 0; number_of_frames--){
       if(!_bitmap_test_bit(location)){
           _bitmap_set_bit(location++);
           used_frames++;
       }
    }
    spinlock_release(&memory_spinlock);
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
    spinlock_acquire(&memory_spinlock);
    for(; number_of_frames > 0; number_of_frames--){
        _bitmap_free_bit(location);
        used_frames--;
    }
    spinlock_release(&memory_spinlock);
}
