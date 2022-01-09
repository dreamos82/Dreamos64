#include <cpu.h>
#include <acpi.h>
#include <video.h>
#include <framebuffer.h>
#include <stdio.h>
#include <rsdt.h>
#include <stddef.h>
#include <string.h>
#include <numbers.h>


RSDT* rsdt_root = NULL;
unsigned int rsdtTablesTotal = 0;

void parse_RSDT(RSDPDescriptor *descriptor){
    printf("Parse RSDP Descriptor\n");
    rsdt_root = (RSDT *) descriptor->RsdtAddress;
    printf("descriptor Address: 0x%x\n", descriptor->RsdtAddress);
    ACPISDTHeader header = rsdt_root->header;
    
    printf("RSDT_Signature: %.4s\n", header.Signature);
    printf("RSDT_Lenght: %d\n", header.Length);
    rsdtTablesTotal = (header.Length - sizeof(ACPISDTHeader)) / sizeof(uint32_t);
    printf("Total rsdt Tables: %d\n", rsdtTablesTotal);
    
    for(int i=0; i < rsdtTablesTotal; i++){
        ACPISDTHeader *tableHeader = (ACPISDTHeader *) rsdt_root->tables[i];
        printf("\tTable header %d: Signature: %.4s\n", i, tableHeader->Signature);
    }
//    _printStr(header.Signature);
//    printf("RSDT Address: 0x%x", root->header);
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
        _getHexString(number, sum);
        _printStr(number);
        _printStr(" ");
    }
    _printNewLine();
    _getHexString(number, sum);
    _printStr("Checksum of RSDP is: ");
    _printStr(number);
    _printNewLine();
    return sum == 0;
}
