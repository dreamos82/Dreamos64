#include <madt.h>
#include <rsdt.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>

MADT_Item* get_MADT_item(MADT* table, uint8_t type) {
    printf("Searching for item type: %d\n", type);
    if(strncmp(table->header.Signature, MADT_ID, 4) == 0) {
        printf("Table is MADT\n");
    }
    MADT_Item* item = (MADT_Item *) (table + sizeof(MADT));
    printf("Address of MADT: %x\n", (uint64_t) table);
    return NULL;
}
