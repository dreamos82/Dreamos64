#include <bitmap.h>
#include <mmap.h>
#include <pmm.h>
#include <multiboot.h>
#include <stdlib.h>
#include <assert.h>
#include <stdio.h>
#include <stdbool.h>
#include <video.h>
#include <inttypes.h>
#include <test_mem.h>
#include <test_common.h>

struct multiboot_tag_basic_meminfo *tagmem;
struct multiboot_tag_mmap *mmap_root;
unsigned long _kernel_physical_end;
extern uint64_t *memory_map;
extern uint32_t number_of_entries;
extern uint32_t bitmap_size;
extern uint32_t used_frames;
extern uint32_t mmap_number_of_entries;
extern multiboot_memory_map_t *mmap_entries;

int main(){
    memory_map = (uint64_t *) malloc(20 * sizeof(uint64_t));
    tagmem = (struct multiboot_tag_basic_meminfo *) malloc(sizeof(struct multiboot_tag_basic_meminfo));
    tagmem->mem_lower = 0x27F;
    tagmem->mem_upper = 0xFFB80;

    _kernel_physical_end = 0x11505C;
    //struct multiboot_tag_mmap *mmap_root;
    uint32_t mmap_size = sizeof(struct multiboot_tag_mmap) + 6*sizeof(struct multiboot_mmap_entry);
    printf("Size: %d\n", mmap_size);
    mmap_root = malloc(mmap_size);
    mmap_root->entries[0].addr = 0;
    mmap_root->entries[0].len = 0x9FC00;
    mmap_root->entries[0].type = 1;
    mmap_root->entries[1].addr = 0x9FC00;
    mmap_root->entries[1].len = 0x400;
    mmap_root->entries[1].type = 2;

//    mmap_data.number_of_entries = 2;
//    mmap_data.entries = mmap_root->entries;
    /*printf("1");
    //multiboot_memory_map_t entries[6];
    printf("1");*/
    mmap_root->entries[2].addr = 0xF000;
    mmap_root->entries[2].len = 0x1000;
    mmap_root->entries[2].type = 2;
    mmap_root->entries[3].addr = 0x100000;
    mmap_root->entries[3].len = 0x3FEE0000;
    mmap_root->entries[3].type = 1;
    mmap_root->entries[4].addr = 0x3FFE0000;
    mmap_root->entries[4].len = 0x20000;
    mmap_root->entries[4].type = 2;
    mmap_root->entries[5].addr = 0xFFFC0000;
    mmap_root->entries[5].len = 0x40000;
    mmap_root->entries[5].type = 2; 
    mmap_root->size = 0xA0;
    mmap_root->entry_size = 0x18;
    mmap_root->entry_version = 0;
    _mmap_parse(mmap_root);
    pmm_setup();
    printf("Testing physical memory manager\n");
    test_pmm();
    test_mmap();
    return 0;
}

void test_pmm(){
    printf("--- Bitmap ---");
    printf("Used frames: 0x%X\n", used_frames);
    printf("--Testing used_frames value\n");
    assert(used_frames==0x2);
    printf("--Testing memory_map initial value\n");
    printf("memory_map[0] = %x\n", memory_map[0]);
    assert(memory_map[0]==0x3);
    printf("--Testing value of first call to _bitmap_request_frame()\n");
    uint64_t frame_value = _bitmap_request_frame();
    assert(frame_value == 0x2);
    assert((frame_value * PAGE_SIZE_IN_BYTES) == 0x400000);
    printf("--Checking that pmm_check_availability is returning true\n");
    bool available_pages = pmm_check_frame_availability();
    assert(available_pages == true);
    printf("--Checking pmm_alloc_frame()\n");
    uint64_t* frame = pmm_alloc_frame();
    printf("used_frames: %x\n", used_frames);
    assert(used_frames == 0x3);
    printf("--Checking that returned address is: 0x%x\n", frame);
    assert(frame == (void*)0x400000);
    assert(memory_map[0] == 0x7);
    printf("--Test setting a frame at row 1 column 3\n");
    _bitmap_set_bit(67);
    assert(memory_map[1] == 0x8);
    printf("--Test setting a frame at row 1 column 6\n");
    _bitmap_set_bit(70);
    assert(memory_map[1] == 0x48);
    printf("--Test freeing a frame at row 1 column 3\n");
    _bitmap_free_bit(67);
    assert(memory_map[1] == 0x40);
    printf("--Trying allocating another frame\n");
    frame = pmm_alloc_frame();
    printf("--Checking that returned address is: 0x%x\n", frame);
    assert(frame == (void*)0x600000);
    assert(used_frames == 0x4);
    assert(memory_map[0] == 0xF);
    printf("--Testing test_bit on frame 67 should be 0\n");
    bool bit_value = _bitmap_test_bit(67);
    assert(bit_value == false);
    printf("--Testing test_bit on frame 70 - should be 1\n");
    bit_value = _bitmap_test_bit(70);
    assert(bit_value == true);
    printf("--Testing test_bit on frame 3 - should be 1\n");
    bit_value = _bitmap_test_bit(70);
    assert(bit_value == true);
    printf("--Trying to free a page\n");
    pmm_free_frame((uint64_t*)0x400000);
    printf("--Memory map value: 0x%X\n", memory_map[0]);
    assert(memory_map[0] == 0xB);
    assert(used_frames == 0x3);
    printf("--Trying to free another frame\n");
    pmm_free_frame(frame);
    assert(used_frames == 0x2);
    printf("used_frames == 0x9 %d\n", used_frames == 0x9);
    printf("Finished\n");
}

void test_mmap(){
    printf("--- Test mmap ---\n");
    printf("--Mmmap data.number_of_entries should be 6 == %x\n", mmap_number_of_entries);
    assert(mmap_number_of_entries == 6);
    printf("--Check that mmap_entries point to mmap_root->entries\n");
    assert(mmap_root->entries == mmap_entries);
    printf("--Check that 0x%X address is correctly set to 1\n", mmap_entries[1].addr);
    uint32_t bitmap_entry = ADDRESS_TO_BITMAP_ENTRY(mmap_entries[1].addr);
    assert(_bitmap_test_bit(bitmap_entry) == true);
    printf("--Check that 0x%X address is correctly set to 1\n", mmap_entries[2].addr);
    bitmap_entry = ADDRESS_TO_BITMAP_ENTRY(mmap_entries[2].addr);
    assert(_bitmap_test_bit(bitmap_entry) == true);
    printf("--Check that 0x%X is correctly set to 1\n", mmap_entries[3].addr);
    bitmap_entry = ADDRESS_TO_BITMAP_ENTRY(mmap_entries[3].addr);
    assert(_bitmap_test_bit(bitmap_entry) == true);
    printf("--Check that 11th bit of bitmap should be set as 0\n");
    bitmap_entry = ADDRESS_TO_BITMAP_ENTRY((mmap_entries[3].addr + 0x1300000));
    assert(_bitmap_test_bit(bitmap_entry) == false);
    printf("Finished\n");
}

