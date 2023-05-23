#ifndef _PMM_H
#define _PMM_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

void pmm_setup(unsigned long addr, uint32_t size);
void _map_pmm();
void *pmm_alloc_frame();
void *pmm_alloc_area(size_t size);
void pmm_free_frame(void *address);
bool pmm_check_frame_availability();

void pmm_reserve_area(uint64_t starting_address, size_t size);
void pmm_free_area(uint64_t starting_address, size_t size);
#endif
