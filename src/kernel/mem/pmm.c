#include <hh_direct_map.h>
#include <kernel.h>
#include <pmm.h>
#include <bitmap.h>
#include <stdbool.h>
#include <mmap.h>
#include <vmm.h>
#include <logging.h>
#include <spinlock.h>
#include <vmm_util.h>

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

bool pmm_initialized = false;
uint64_t anon_memory_loc;
uint64_t anon_physical_memory_loc;

void pmm_setup(uint64_t addr, uint32_t size){
    // addr = address of multiboot structre
    // size = size of the structure
    pretty_logf(Verbose, "addr: 0x%x, size: 0x%x", addr,size);
    // anon_memory_loc is the memory just after the kernel end (virtual address)
    // anon_physical_memory_loc is the physical address
    anon_memory_loc = (uint64_t) align_up( (size_t) (&_kernel_end + PAGE_SIZE_IN_BYTES), PAGE_SIZE_IN_BYTES);
    anon_physical_memory_loc = (uint64_t) align_up( (size_t) (&_kernel_physical_end + PAGE_SIZE_IN_BYTES), PAGE_SIZE_IN_BYTES );

    pretty_logf(Verbose, "anon_memory_loc: 0x%x, anon_physical_memory_loc: 0x%x", anon_memory_loc, anon_physical_memory_loc);
    // This is similar to a chicken-egg problem, we need to initialize the physical memory manager, but for doing it we need to map some physical memory into the virtual space,
    // But the mapping functions relies on the physical memory manager.
    // The first thing we want to do is initialize the hhdm, since it is needed to for the memory mapping function to work properly
    hhdm_map_physical_memory();
    pretty_log(Verbose, "HHDM setup finished");

    _initialize_bitmap(anon_physical_memory_loc + PAGE_SIZE_IN_BYTES);
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
    _mmap_setup();
    pmm_initialized = true;
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


void *pmm_prepare_new_pagetable() {
    if ( !pmm_initialized) {
        while((!_mmap_is_address_in_available_space(anon_physical_memory_loc, PAGE_DIR_SIZE)) && anon_physical_memory_loc < memory_size_in_bytes) {
            anon_memory_loc += PAGE_DIR_SIZE;
            anon_physical_memory_loc += PAGE_DIR_SIZE;
        }
        //pretty_logf(Verbose, "Preparing new page table at: 0x%x - phys: 0x%x", anon_memory_loc, anon_physical_memory_loc);
        anon_memory_loc += PAGE_DIR_SIZE;
        anon_physical_memory_loc += PAGE_DIR_SIZE;
        return (void *)  (anon_physical_memory_loc - PAGE_DIR_SIZE);
    }
    return (void *) pmm_alloc_frame();
}

void *pmm_alloc_area(size_t size) {
    size_t requested_frames = get_number_of_pages_from_size(size);

    pretty_logf(Verbose, "requested_frames: %x\n", requested_frames);
    spinlock_acquire(&memory_spinlock);
    uint64_t frames = _bitmap_request_frames(requested_frames);

    for (size_t i =0; i < requested_frames; i++) {
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
