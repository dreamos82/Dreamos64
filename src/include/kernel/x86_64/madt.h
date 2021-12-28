#ifndef _MADT_H
#define _MADT_H

#include <stdint.h>
typedef struct MADT_Item {
    uint8_t type;
    uint8_t length;
} MADT_Item;

MADT_Item* get_MADT_item(char *);
#endif
