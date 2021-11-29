#include <mmap.h>
#include <bitmap.h>
#include <pmm.h>
#include <stdint.h>
#include <multiboot.h>
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

void _mmap_parse(struct multiboot_tag_mmap *mmap_root){
    int total_entries = 0;
    mmap_number_of_entries = (mmap_root->size - sizeof(*mmap_root))/mmap_root->entry_size;
    mmap_entries = (multiboot_memory_map_t *)mmap_root->entries;
    uint32_t i=0;

#ifndef _TEST_
    while(i<mmap_number_of_entries){
        _printStringAndNumber("Address: ", (uint32_t)mmap_entries[i].addr);
        _printStringAndNumber("---Len: ", (uint32_t)mmap_entries[i].len);
        _printStringAndNumber("---Type:: ", mmap_entries[i].type);
        _printStr("Type str: ");
        _printStr((char *) mmap_types[mmap_entries[i].type]);
        _printNewLine();
        _printStringAndNumber("---zero:: ", mmap_entries[i].zero);
        _printStr("END OF MMAP ITEM\n");
        total_entries++;
        i++;
    }
    _printStringAndNumber("Total entries: ", total_entries);
    _printStringAndNumber("double check: ", (mmap_root->size - sizeof(*mmap_root))/mmap_root->entry_size);
#endif
}

void _mmap_setup(){
    count_physical_reserved=0;
    if(used_frames > 0){
        uint32_t counter = 0;
        uint64_t mem_limit = (tagmem->mem_upper + 1024) * 1024;
        while(counter < mmap_number_of_entries){
            if(mmap_entries[counter].addr < mem_limit &&
                    mmap_entries[counter].type > 1){
               _printStringAndNumber("Found entry at addr: ", mmap_entries[counter].addr);
               pmm_reserve_area(mmap_entries[counter].addr, mmap_entries[counter].len);
               count_physical_reserved++;
            }
            counter++;
        }
    }
}

bool _is_address_reserved_in_mmap(uint64_t address){
    int i = 0;
    if(mmap_number_of_entries == 0){
        return false;
    }
    while (i < mmap_number_of_entries){
        if(address > mmap_entries[i].addr && address < mmap_entries[i].addr + mmap_entries[i].len){
            if(mmap_entries[i].type != _MMAP_AVAILABLE)
                return true;
        }
        i++;
    }
    return false;
}

