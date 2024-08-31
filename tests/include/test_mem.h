#ifndef _TEST_MEM_H
#define _TEST_MEM_H

extern uint64_t multiboot_tag_end;
extern uint64_t multiboot_tag_start;

void test_pmm_initialize();
void test_pmm();
void test_mmap();

#endif

