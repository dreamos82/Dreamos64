#ifndef __VMM_H
#define __VMM_H

#include <stddef.h>

#define PRESENT 1
#define WRITE_ENABLE 2
#define USER_LEVEL 4

void *map_vaddress(void *, unsigned int);
int unmap_vaddress(void *);

#endif
