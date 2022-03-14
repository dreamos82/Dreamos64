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
XSDT* xsdt_root = NULL;

unsigned int rsdtTablesTotal = 0;

void parse_SDT(uint64_t address, uint8_t type) {
    if ( type == RSDT_V1) {
        parse_RSDT((RSDPDescriptor *) address);
    } else if ( type == RSDT_V2 ) {
        parse_RSDTv2((RSDPDescriptor20 *) address);
    }    
}

void parse_RSDT(RSDPDescriptor *descriptor){
    printf("- Parse RSDP Descriptor\n");
    printf("- descriptor Address: 0x%x\n", descriptor->RsdtAddress);
    map_phys_to_virt_addr((void *) descriptor->RsdtAddress, (void *) ensure_address_in_higher_half(descriptor->RsdtAddress), 0);
    rsdt_root = (RSDT *) ensure_address_in_higher_half((uint64_t) descriptor->RsdtAddress);
    printf("- RSDT_Address: %x\n", (uint64_t) ensure_address_in_higher_half(descriptor->RsdtAddress));
    ACPISDTHeader header = rsdt_root->header;
    printf("- RSDT_Signature: %.4s\n", header.Signature);
    printf("- RSDT_Lenght: %d\n", header.Length);
    // Ok we are here and we have mapped the "head of rsdt", it will stay most likely in one page, but there is no way
    // to know the length of the whole table before mapping its header. So now we are able to check if we need to map extra pages
    size_t required_extra_pages = (header.Length / KERNEL_PAGE_SIZE) + 1;
    printf("- RSDT_PAGES_NEEDED: %d\n", required_extra_pages);
    if (required_extra_pages > 1) {
        printf("- Mapping extra pages");
        for (int j = 1; j < required_extra_pages; j++) {
            uint64_t new_physical_address = descriptor->RsdtAddress + (j * KERNEL_PAGE_SIZE);
            map_phys_to_virt_addr(new_physical_address, ensure_address_in_higher_half(new_physical_address), 0);
        }
    }
    rsdtTablesTotal = (header.Length - sizeof(ACPISDTHeader)) / sizeof(uint32_t);
    printf("- Total rsdt Tables: %d\n", rsdtTablesTotal);
    
    for(int i=0; i < rsdtTablesTotal; i++) {
        ACPISDTHeader *tableHeader = (ACPISDTHeader *) rsdt_root->tables[i];
        printf("\tTable header %d: Signature: %.4s\n", i, tableHeader->Signature);
    }
}

void parse_RSDTv2(RSDPDescriptor20 *descriptor){
    printf("Parse RSDP v2 Descriptor\n");
    printf("- Descriptor address: 0x%x\n", descriptor->XsdtAddress);
    map_phys_to_virt_addr((void *) (descriptor->XsdtAddress & (~0x1fffffl)), (void *) ensure_address_in_higher_half(descriptor->XsdtAddress), 0);
    printf("- RSDTv2_Address: %x\n",  (uint64_t) ensure_address_in_higher_half(descriptor->XsdtAddress));
    xsdt_root = (XSDT *) ensure_address_in_higher_half((uint64_t) descriptor->XsdtAddress);
    printf("- RSDTv2_Length: 0x%x\n", xsdt_root);
    printf("- RSDTv2_Length: 0x%x\n", descriptor->Length);
    ACPISDTHeader header = xsdt_root->header;
    printf("- XSDT_Signature: %.4s\n", header.Signature);
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

/*! \brief It validate the RSDP (v1 and v2) checksum
 * 
 * Given the descriptor as a byte array, it sums each byte, and if the last byte of the sum is 0 this means that the structure is valid.
 * @param decriptor the RSDPv1/v2 descriptor
 * @param the size of the struct used by descriptor (should be the size of: RSDPDescriptor or RSDPDescriptorv2)
 * @return true if the validation is succesfull
 */
bool validate_RSDP(char *descriptor, size_t size){
    uint32_t sum = 0;
    printf("Bytes: ");
    for (uint32_t i=0; i < size; i++){
        sum += ((char*) descriptor)[i];
        printf("%x - ", sum); 
    }
    printf("End\n");
    printf("Checksum of RSDP is: 0x%x\n", sum&0xFF);
    return (sum&0xFF == 0);
}
