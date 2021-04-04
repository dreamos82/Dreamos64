#include <bitmap.h>
#include <multiboot.h>
#include <main.h>
#include <video.h>

extern struct multiboot_tag_basic_meminfo *tagmem;
extern unsigned long _kernel_physical_end;
void _initialize_bitmap(){
    //TODO
    //1. get upper memory
    uint32_t memory_size_in_bytes = tagmem->mem_upper * 1024;
    uint32_t bitmap_size = memory_size_in_bytes / PAGE_SIZE_IN_BYTES;
    uint32_t number_of_entries = bitmap_size / 64;
    _printStringAndNumber("Actual size in bytes: ", memory_size_in_bytes);
    _printStringAndNumber("Number of bit entries: ", bitmap_size);
    _printStringAndNumber("Number items: ", number_of_entries);
}
