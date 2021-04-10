#ifndef _PMM_H
#define _PMM_H

#include <stdint.h>
#include <stdbool.h>

uint64_t pmm_alloc_frame();
bool pmm_check_frame_availability();
#endif
