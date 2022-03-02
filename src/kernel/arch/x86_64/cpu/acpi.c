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

RSDT* rsdt_root = NULL;
unsigned int rsdtTablesTotal = 0;

void parse_RSDT(RSDPDescriptor *descriptor){
    printf("Parse RSDP Descriptor\n");
    printf("descriptor Address: 0x%x\n", descriptor->RsdtAddress);
    map_phys_to_virt_addr(descriptor->RsdtAddress, ensure_address_in_higher_half(descriptor->RsdtAddress), 0);
    rsdt_root = (RSDT *) ensure_address_in_higher_half((uint64_t) descriptor->RsdtAddress);
    printf("RSDT_Address: %x\n", ensure_address_in_higher_half(descriptor->RsdtAddress));
    ACPISDTHeader header = rsdt_root->header;
    printf("RSDT_Signature: %.4s\n", header.Signature);
    printf("RSDT_Lenght: %d\n", header.Length);
    rsdtTablesTotal = (header.Length - sizeof(ACPISDTHeader)) / sizeof(uint32_t);
    printf("Total rsdt Tables: %d\n", rsdtTablesTotal);
    
    for(int i=0; i < rsdtTablesTotal; i++){
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
