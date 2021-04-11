#ifndef _PMM_H
#define _PMM_H

#include <stdint.h>
#include <stdbool.h>

void *pmm_alloc_frame();
void pmm_free_frame(void*);
bool pmm_check_frame_availability();
#endif
