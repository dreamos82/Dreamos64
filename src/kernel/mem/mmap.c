#include <mmap.h>
#include <stdint.h>


mmap_wrapper mmap_data;

void _parse_mmap(struct multiboot_tag_mmap *mmap_root){
    uint32_t entries = mmap_root->size - sizeof(*mmap_root);
    struct multiboot_mmap_entry* mmap_entry = (struct multiboot_mmap_entry *)mmap_root->entries;
    int total_entries = 0;
    mmap_data.number_of_entries = (mmap_root->size - sizeof(*mmap_root))/mmap_root->entry_size;
    mmap_data.entries = mmap_root->entries;
    int i=0;
    while(i<mmap_data.number_of_entries){
        _printStringAndNumber("Address: ", (uint32_t)mmap_data.entries[i].addr);
        _printStringAndNumber("---Len: ", (uint32_t)mmap_data.entries[i].len);
        _printStringAndNumber("---Type:: ", mmap_data.entries[i].type);
        _printStr("Type str: ");
        _printStr(mmap_types[mmap_data.entries[i].type]);
        _printNewLine();
        _printStringAndNumber("---zero:: ", mmap_data.entries[i].zero);
//        mmap_entry = (struct multiboot_mmap_entry*)((uintptr_t)mmap_entry + mmap_root->entry_size);
        total_entries++;
        i++;
//        entries -= mmap_root->entry_size;
    }
    _printStringAndNumber("Total entries: ", total_entries);
    _printStringAndNumber("double check: ", (mmap_root->size - sizeof(*mmap_root))/mmap_root->entry_size);


}
