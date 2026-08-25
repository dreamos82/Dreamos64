#include <bitmap.h>
#include <mmap.h>
#include <kernel.h>
#include <test_mem.h>
#include <test_common.h>
#include <test_stats.h>
#include <pmm.h>
#include <multiboot.h>
#include <stdlib.h>
#include <assert.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <video.h>
#include <inttypes.h>
#include <main.h>
#include <unistd.h>

extern uint64_t *memory_map;
extern uint32_t number_of_entries;
extern uint32_t bitmap_size;
extern uint32_t used_frames;
extern uint32_t mmap_number_of_entries;
extern multiboot_memory_map_t *mmap_entries;

struct multiboot_tag_basic_meminfo *tagmem;
struct multiboot_tag_mmap *mmap_root;

bool has_pipe = false;
test_runner_t *tests;
unsigned int number_of_tests = 0;

//unsigned long _kernel_physical_end __attribute__((section(".mySection"))) = 0x9ABCDEF0;
uint64_t _kernel_end = 0x1190AC;
uint64_t _kernel_physical_end = 0x1190AC;

int main(int argc, char **argv) {
    int pipe_fd = -1;
    if (argc > 1) {
            //pipe_fd = atoi(argv[1]);
            has_pipe = true;
            pipe_fd = atoi(argv[1]);
            printf("Found pipe id: %d\n", pipe_fd);
    }    
    multiboot_tag_start = 0x1ca000;
    multiboot_tag_start = 0x9bf;    
    prepare_tests();
    for (int i = 0; i < number_of_tests; i++) {
        printf("%d) %s:\n", tests[i].stats.module_id, tests[i].stats.module_title);
        tests[i].handler(i);
        print_stats(tests[i].stats);
    }
    if ( has_pipe ) {
        printf("%d\n", number_of_tests);
        write(pipe_fd, &number_of_tests, sizeof(number_of_tests));
        for (int j=0; j<number_of_tests; j++) {
            send_stats(pipe_fd, tests[j].stats);
        }
        close(pipe_fd);
    }
    return 0;
}

void prepare_tests() {
    printf("Testing physical memory manager\n");
    printf("===============================\n\n");
    test_pmm_initialize();
    test_init(2, &tests);
    add_test(1, "Testing Memory Bitmap", test_pmm, tests);
    number_of_tests++;
    add_test(2, "Testing Memory Map", test_mmap, tests);
    number_of_tests++;
}

void test_pmm_initialize(){
    uint32_t bitmap_entries = _compute_kernel_entries(_kernel_end);
    memory_size_in_bytes = 20 * sizeof(uint64_t);
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
    pmm_setup((unsigned long) memory_map, 20 * sizeof(uint64_t));

    _mmap_setup();
}

void test_pmm(unsigned int id){
    pretty_assert_stat(0x2, used_frames, ==, tests[id].stats, "Checking initial value of used_frames", has_pipe);    
    pretty_assert_stat(0x3, memory_map[0], ==, tests[id].stats, "Testing memory_map[0] value after init", has_pipe);
    uint64_t frame_value = _bitmap_request_frame();
    pretty_assert_stat(0x2, frame_value, ==, tests[0].stats, "Testing value of first call to _bitmap_request_frame()" , has_pipe);
    pretty_assert_stat(0x400000, frame_value * PAGE_SIZE_IN_BYTES, ==, tests[id].stats, "Testing result of frame_value * PAGE_SIZE_BYTES", has_pipe);    
    bool available_pages = pmm_check_frame_availability();
    pretty_assert_stat(true, available_pages, ==, tests[id].stats, "Checking that pmm_check_availability is returning true", has_pipe);
    uint64_t  frame = (uint64_t) pmm_alloc_frame();
    pretty_assert_stat(0x3, used_frames, ==, tests[id].stats, "Checking used_frames after pmm_alloc_frame call", has_pipe);
    pretty_assert_stat((void*)0x400000, (void *)frame, ==, tests[id].stats, "Checking returned address", has_pipe);
    pretty_assert_stat(0x7, memory_map[0], ==, tests[id].stats, "Checking memory_map value", has_pipe);    
    _bitmap_set_bit(67);
    pretty_assert_stat(0x8, memory_map[1], ==, tests[id].stats, "Test setting a frame at row 1 column 3 (=67)", has_pipe);
    _bitmap_set_bit(70);
    pretty_assert_stat(0x48, memory_map[1], ==, tests[id].stats, "Test setting a frame at row 1 column 6 (=70)", has_pipe);
    _bitmap_free_bit(67);
    pretty_assert_stat(0x40, memory_map[1], ==, tests[id].stats, "Test freeing a frame at row 1 column 3", has_pipe);
    frame = (uint64_t)pmm_alloc_frame();
    pretty_assert_stat((void*)0x600000, (void *)frame, ==, tests[id].stats, "Test returned address after a new frame allocation", has_pipe);
    pretty_assert_stat(0xF, memory_map[0], ==, tests[id].stats, "Testing value of memory_map[0]", has_pipe);
    bool bit_value = _bitmap_test_bit(67);
    pretty_assert_stat(false, bit_value, ==, tests[id].stats, "Testing _bitmap_test_bit on frame 67 that should be false", has_pipe);
    bit_value = _bitmap_test_bit(70);
    pretty_assert_stat(true, bit_value, ==, tests[id].stats, "Testing _bitmap_test_bit, on frame 70 that should be true", has_pipe);
    bit_value = _bitmap_test_bit(3);
    pretty_assert_stat(true, bit_value, ==, tests[id].stats, "Testing _bitmap_test_bit, on frame 3 that should be true", has_pipe);
    pmm_free_frame( (uint64_t*)0x400000 );
    pretty_assert_stat(0xB, memory_map[0],  ==, tests[id].stats, "Test memory_map[0], after pmm_free_frame(0x400000)", has_pipe);
    pretty_assert_stat(0x3, used_frames, ==, tests[id].stats, "Checking used frame after pmm_free_frame", has_pipe);
    pmm_free_frame((uint64_t *) frame);
    pretty_assert_stat(0x3, memory_map[0],  ==, tests[id].stats, "Trying to free another frame", has_pipe);
    pretty_assert_stat(0x2, used_frames, ==, tests[id].stats, "Checking used frame after pmm_free_frame", has_pipe);
    uint64_t *frame_2 = pmm_alloc_area(0x250000);
    pretty_assert_stat(0x04, used_frames, ==, tests[id].stats, "Checking used frame after pmm_alloc_frame", has_pipe);
    pretty_assert_stat((uint64_t *)0x400000, frame_2, ==, tests[id].stats, "Checking returned address", has_pipe);    
    memory_map[0] = 75;
    used_frames = 4;
    frame_2 = pmm_alloc_area(0x500000);
    pretty_assert_stat(0x3CB, memory_map[0], ==, tests[id].stats, "Trying to allocate an area after changin the memory map", has_pipe);
    pretty_assert_stat(0x7, used_frames, ==, tests[id].stats, "Checking used frame after pmm_free_frame", has_pipe);
    printf("Finished\n");
}

void test_mmap(unsigned int id){
    pretty_assert_stat(6, mmap_number_of_entries, ==, tests[id].stats, "Mmap data.number_of_entries should be 6", has_pipe  );
    pretty_assert_stat(mmap_entries, mmap_root->entries, ==, tests[id].stats, "Check that mmap_entries point to mmap_root->entries", has_pipe);
    uint32_t bitmap_entry = ADDRESS_TO_BITMAP_ENTRY(mmap_entries[1].addr);
    pretty_assert_stat(true, _bitmap_test_bit(bitmap_entry), ==, tests[id].stats, "Check that address at mmap_entries[2].addrs is set to 1", has_pipe);
    bitmap_entry = ADDRESS_TO_BITMAP_ENTRY(mmap_entries[2].addr);
    pretty_assert_stat(true, _bitmap_test_bit(bitmap_entry), ==, tests[2].stats, "Check that address at mmap_entries[0].addrs is set to 1", has_pipe);
    bitmap_entry = ADDRESS_TO_BITMAP_ENTRY(mmap_entries[3].addr);
    pretty_assert_stat(true, _bitmap_test_bit(bitmap_entry), ==, tests[id].stats, "Check that address at mmap_entries[3].addrs is set to 1", has_pipe);
    bitmap_entry = ADDRESS_TO_BITMAP_ENTRY((mmap_entries[3].addr + 0x1300000));
    pretty_assert_stat(false, _bitmap_test_bit(bitmap_entry), ==, tests[id].stats, "Check that eleventh bit of the bitmap should be 0", has_pipe);
    pmm_free_area(mmap_entries[1].addr, mmap_entries[1].len);
    bitmap_entry = ADDRESS_TO_BITMAP_ENTRY(mmap_entries[1].addr);
    pretty_assert_stat(true, _bitmap_test_bit(bitmap_entry), ==, tests[id].stats, "Trying to free area ", has_pipe);
    pmm_free_area(mmap_entries[2].addr, mmap_entries[2].len);
    bitmap_entry = ADDRESS_TO_BITMAP_ENTRY(mmap_entries[2].addr);    
    pretty_assert_stat(true, _bitmap_test_bit(bitmap_entry), ==, tests[id].stats, "Try to free another area that should not happen", has_pipe);
    printf("Finished\n");
}

