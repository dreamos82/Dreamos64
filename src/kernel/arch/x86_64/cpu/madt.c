#include <madt.h>
#include <rsdt.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>

MADT_Item* get_MADT_item(MADT* table, uint8_t type, uint8_t offset) {
    printf("Searching for item type: %d\n", type);
    if(strncmp(table->header.Signature, MADT_ID, 4) == 0) {
        printf("Table is MADT\n");
    }
    MADT_Item* item = (MADT_Item *) ((uint32_t)table + sizeof(MADT));
    printf("Address of MADT: %x\n", (uint64_t) table);
    int total_length = sizeof(MADT);
    uint8_t counter = 0;
    while(total_length < table->header.Length && counter <= offset) {
        //printf("Item found: 0x%x - Length: 0x%x\n", item->type, item->length);
        if(item->type == type) {
            if(counter ==  offset) {
                return item;
            }
            counter++;
        }
        total_length = total_length + item->length;        
        item = (MADT_Item *)((uint32_t)table + total_length);
    }
    return NULL;
}
