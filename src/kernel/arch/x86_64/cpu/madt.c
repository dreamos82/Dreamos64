#include <madt.h>
#include <rsdt.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include <vm.h>
#include <bitmap.h>
#include <vmm.h>
#include <logging.h>

bool is_madt_mapped;
MADT_Item *madt_base = NULL;

void map_madt(MADT* table){
    if(strncmp(table->header.Signature, MADT_ID, 4) != 0) {
        // Table is not MADT
        return;
    }
    
    uint64_t madt_address = ((uint64_t) table + sizeof(MADT));
    map_phys_to_virt_addr((void *) ALIGN_PHYSADDRESS(madt_address), (void *) ensure_address_in_higher_half(madt_address), 0);
    _bitmap_set_bit_from_address(ALIGN_PHYSADDRESS(madt_address));
    loglinef(Verbose, "(map_madt): Sizeof MADT struct: 0x%x", sizeof(MADT));
    madt_base = (MADT_Item *) ensure_address_in_higher_half((uint64_t)madt_address);
    is_madt_mapped = true;
}

MADT_Item* get_MADT_item(MADT* table, uint8_t type, uint8_t offset) {
    if ( madt_base == NULL ) {
        map_madt(table);
    }
    if ( strncmp(table->header.Signature, MADT_ID, 4) != 0 ) {
        return NULL;
    }
    MADT_Item* item = (MADT_Item *) madt_base;
    uint64_t total_length = 0;
    uint8_t counter = 0;
    while (total_length + sizeof(MADT) < table->header.Length && counter <= offset ) {
        //loglinef(Verbose, "Item found: 0x%x - Length: 0x%x - total_length: %x\n", item->type, item->length, total_length);
        if (item->type == type) {
            // Should it return null if offset > #items of required type? or return the last one found?
            if(counter ==  offset) {
                return item;
            }
            counter++;
        }
        total_length = total_length + item->length;        
        item = (MADT_Item *)((uint64_t)madt_base + (uint64_t) total_length);
    }
    return NULL;
}

void print_madt_table(MADT* table) {
    if ( madt_base == NULL ) {
        map_madt(table);
    }

    //logline(Verbose, "Printing madt table: ");
    uint32_t total_length = 0;
    MADT_Item* item = (MADT_Item *) (madt_base);
    int counter = 0;
    while(total_length + sizeof(MADT) < table->header.Length) {
        //loglinef(Verbose, "    %d: Type: 0x%x - Length: 0x%x\n", counter, item->type, item->length, item, total_length, ((uint64_t)madt_base + (uint64_t) total_length));
        counter++;
        total_length = total_length + item->length;
        item = (MADT_Item *)((uint64_t)madt_base + (uint64_t) total_length);
    }
}
