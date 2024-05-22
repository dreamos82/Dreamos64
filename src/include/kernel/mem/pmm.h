#ifndef _PMM_H
#define _PMM_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

extern bool pmm_initialized;

void pmm_setup(uint64_t addr, uint32_t size);
void *pmm_prepare_new_pagetable();
void *pmm_alloc_frame();
void *pmm_alloc_area(size_t size);
void pmm_free_frame(void *address);
bool pmm_check_frame_availability();

void pmm_reserve_area(uint64_t starting_address, size_t size);
void pmm_free_area(uint64_t starting_address, size_t size);

#endif
