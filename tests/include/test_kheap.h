#ifndef __TEST_KHEAP_
#define __TEST_KHEAP_

#define PAGE_SIZE 0x1000
void test_kmalloc(unsigned int id);
void test_kfree(unsigned int id);
void prepare_tests();
#endif
