#ifndef _PMM_H
#define _PMM_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

void pmm_setup(unsigned long, uint32_t);
void *pmm_alloc_frame();
void pmm_free_frame(void*);
bool pmm_check_frame_availability();

void pmm_reserve_area(uint64_t, size_t);
void pmm_free_area(uint64_t, size_t);
#endif
