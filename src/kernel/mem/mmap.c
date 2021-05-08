#include <mmap.h>
#include <bitmap.h>
#include <pmm.h>
#include <stdint.h>
#include <multiboot.h>
#ifndef _TEST_
#include <video.h>
#endif

extern uint32_t used_frames;
extern struct multiboot_tag_basic_meminfo *tagmem;
mmap_wrapper mmap_data;

const char *mmap_types[] = {
    "Invalid",
    "Available",
    "Reserved",
    "Reclaimable",
    "NVS",
    "Defective"
};

void _mmap_parse(struct multiboot_tag_mmap *mmap_root){
    int total_entries = 0;
    mmap_data.number_of_entries = (mmap_root->size - sizeof(*mmap_root))/mmap_root->entry_size;
    mmap_data.entries = mmap_root->entries;
#ifndef _TEST_
    uint32_t i=0;
    while(i<mmap_data.number_of_entries){
        _printStringAndNumber("Address: ", (uint32_t)mmap_data.entries[i].addr);
        _printStringAndNumber("---Len: ", (uint32_t)mmap_data.entries[i].len);
        _printStringAndNumber("---Type:: ", mmap_data.entries[i].type);
        _printStr("Type str: ");
        _printStr((char *) mmap_types[mmap_data.entries[i].type]);
        _printNewLine();
        _printStringAndNumber("---zero:: ", mmap_data.entries[i].zero);
        total_entries++;
        i++;
    }
    _printStringAndNumber("Total entries: ", total_entries);
    _printStringAndNumber("double check: ", (mmap_root->size - sizeof(*mmap_root))/mmap_root->entry_size);
#endif
}

void _mmap_setup(){
    if(used_frames > 0){
        uint32_t counter = 0;
        uint64_t mem_limit = (tagmem->mem_upper + 1024) * 1024;
        while(counter < mmap_data.number_of_entries){
            if(mmap_data.entries[counter].addr < mem_limit &&
                    mmap_data.entries[counter].type > 1){
               _printStringAndNumber("Found entry at addr: ", mmap_data.entries[counter].addr);
               pmm_reserve_area(mmap_data.entries[counter].addr, mmap_data.entries[counter].len);
            }
            counter++;
        }
    }
}
