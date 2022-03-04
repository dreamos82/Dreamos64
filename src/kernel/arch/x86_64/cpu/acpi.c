#include <cpu.h>
#include <acpi.h>
#include <video.h>
#include <framebuffer.h>
#include <stdio.h>
#include <rsdt.h>
#include <stddef.h>
#include <string.h>
#include <numbers.h>
#include <vmm.h>
#include <kheap.h>

RSDT* rsdt_root = NULL;
unsigned int rsdtTablesTotal = 0;

void parse_RSDT(RSDPDescriptor *descriptor){
    printf("Parse RSDP Descriptor\n");
    printf("descriptor Address: 0x%x\n", descriptor->RsdtAddress);
    map_phys_to_virt_addr((void *) descriptor->RsdtAddress, (void *) ensure_address_in_higher_half(descriptor->RsdtAddress), 0);
    rsdt_root = (RSDT *) ensure_address_in_higher_half((uint64_t) descriptor->RsdtAddress);
    printf("RSDT_Address: %x\n", ensure_address_in_higher_half(descriptor->RsdtAddress));
    ACPISDTHeader header = rsdt_root->header;
    printf("RSDT_Signature: %.4s\n", header.Signature);
    printf("RSDT_Lenght: %d\n", header.Length);
    // Ok we are here and we have mapped the "head of rsdt", it will stay most likely in one page, but there is no way
    // to know the length of the whole table before mapping its header. So now we are able to check if we need to map extra pages
    size_t required_extra_pages = (header.Length / KERNEL_PAGE_SIZE) + 1;
    printf("RSDT_PAGES_NEEDED: %d\n", required_extra_pages);
    if (required_extra_pages > 1) {
        printf("Mapping extra pages");
        for (int j = 1; j < required_extra_pages; j++) {
            uint64_t new_physical_address = descriptor->RsdtAddress + (j * KERNEL_PAGE_SIZE);
            map_phys_to_virt_addr(new_physical_address, ensure_address_in_higher_half(new_physical_address), 0);
        }
    }
    rsdtTablesTotal = (header.Length - sizeof(ACPISDTHeader)) / sizeof(uint32_t);
    printf("Total rsdt Tables: %d\n", rsdtTablesTotal);
    
    for(int i=0; i < rsdtTablesTotal; i++) {
        ACPISDTHeader *tableHeader = (ACPISDTHeader *) rsdt_root->tables[i];
        printf("\tTable header %d: Signature: %.4s\n", i, tableHeader->Signature);
    }
}

ACPISDTHeader* get_RSDT_Item(char* table_name) {
    if(rsdt_root == NULL) {
        return NULL;
    }    
    for(int i=0; i < rsdtTablesTotal; i++){
        ACPISDTHeader *tableItem = (ACPISDTHeader *) rsdt_root->tables[i];
        int return_value = strncmp(table_name, tableItem->Signature, 4);
        if(return_value == 0) {
            printf("Item Found...\n");
            return tableItem;
        }
    }
    printf("\n");
    return NULL;
}

void parse_RSDTv2(RSDPDescriptor20 *descriptor){
    printf("Parse RSDP v2 Descriptor");
}

int validate_RSDP(RSDPDescriptor *descriptor){
    uint8_t sum = 0;
    char number[30];

    for (uint32_t i=0; i < sizeof(RSDPDescriptor); i++){
        sum += ((char*) descriptor)[i];
        _getHexString(number, sum, false);
        _printStr(number);
        _printStr(" ");
    }
    _printNewLine();
    _getHexString(number, sum, false);
    _printStr("Checksum of RSDP is: ");
    _printStr(number);
    _printNewLine();
    return sum == 0;
}
