#ifndef _TEST_MEM_H
#define _TEST_MEM_H

extern uint64_t multiboot_tag_end;
extern uint64_t multiboot_tag_start;

void test_pmm_initialize();
void test_pmm(unsigned int id);
void test_mmap(unsigned int id);
void prepare_tests();

#endif

