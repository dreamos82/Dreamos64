#include <bitmap.h>
#include <logging.h>
#include <mmap.h>
#include <multiboot.h>
#include <pmm.h>
#include <stdint.h>
#include <utils.h>
#include <video.h>

extern uint32_t used_frames;
extern struct multiboot_tag_basic_meminfo *tagmem;
uint32_t mmap_number_of_entries;
multiboot_memory_map_t *mmap_entries;
uint8_t count_physical_reserved;

const char *mmap_types[] = {
    "Invalid",
    "Available",
    "Reserved",
    "Reclaimable",
    "NVS",
    "Defective"
};

// This function is apparently only printing the list of mmap items and their value.
void _mmap_parse(struct multiboot_tag_mmap *mmap_root){
    int total_entries = 0;
    mmap_number_of_entries = (mmap_root->size - sizeof(*mmap_root))/mmap_root->entry_size;
    mmap_entries = (multiboot_memory_map_t *)mmap_root->entries;
    uint32_t i=0;

#ifndef _TEST_
    while(i<mmap_number_of_entries){
        pretty_logf(Verbose, "\t[%d] Address: 0x%x - Len: 0x%x Type: (%d) %s", i, mmap_entries[i].addr, mmap_entries[i].len, mmap_entries[i].type, (char *) mmap_types[mmap_entries[i].type]);
        total_entries++;
        i++;
    }
    pretty_logf(Verbose, "Total entries: %d", total_entries);
#endif
}

void _mmap_setup(){
    //TODO: see issue: https://github.com/dreamos82/Dreamos64/issues/209
    count_physical_reserved=0;
    if(used_frames > 0){
        uint32_t counter = 0;
        uint64_t mem_limit = (tagmem->mem_upper + 1024) * 1024;
        while(counter < mmap_number_of_entries){
            if(mmap_entries[counter].addr < mem_limit &&
                    mmap_entries[counter].type > 1){
                pretty_logf(Verbose, "\tFound unusable entry at addr: %x", mmap_entries[counter].addr);
                pmm_reserve_area(mmap_entries[counter].addr, mmap_entries[counter].len);
                count_physical_reserved++;
            }
            counter++;
        }
    }
}

bool _mmap_is_address_in_available_space(uint64_t address, uint64_t upper_limit) {
    for (size_t i=0; i < mmap_number_of_entries; i++) {
        multiboot_memory_map_t* current_entry = &mmap_entries[i];
        if(current_entry->addr + current_entry->len < address + upper_limit) {
            if(current_entry->type == _MMAP_AVAILABLE) {
                //pretty_logf(Verbose, "Entry 0x%x is in an available space (with size: 0x%x", address, upper_limit );
                // The address is in an available area, but we need to check if it is not overwriting something important.
                uint64_t multiboot_address =_is_address_in_multiboot(address);
                if(multiboot_address != 0) {
                    pretty_log(Verbose, " This address is reserved by multiboot");
                    return false;
                }
                return true;
            }
        }
    }
    return false;
}

uintptr_t _mmap_determine_bitmap_region(uint64_t lower_limit, size_t bytes_needed){
    //NOTE: lower_limit can be used to place the bitmap after the kernel, or after anything if need be.
    for (size_t i = 0; i < mmap_number_of_entries; i++){
        multiboot_memory_map_t* current_entry = &mmap_entries[i];

        if (current_entry->type != _MMAP_AVAILABLE)
            continue;
        if (current_entry->addr + current_entry->len < lower_limit)
            continue; //mmap area is too low to be relevant, ignore it

        //there's at some overlap, now check if there is enough pages within the region
        //const size_t pages_needed = bytes_needed / PAGE_SIZE_IN_BYTES + 1; //potential for a lot of wasted space, depending on page size.
        size_t actual_available_space = current_entry->len;
        const size_t entry_offset = lower_limit > current_entry->addr ? lower_limit - current_entry->addr : 0;

        actual_available_space -= entry_offset; //lower limit might force us to be midway through a region

        if (actual_available_space >= bytes_needed)
        {
            pretty_logf(Verbose, "Found space for bitmap at address: 0x%x requested size: 0x%x", current_entry->addr + entry_offset, bytes_needed);
            return current_entry->addr + entry_offset;
        }
    }

    //BOOM! D:
    pretty_log(Fatal, "Could not find a space big enough to hold the pmm bitmap! This should not happen.");
    return 0;
}
