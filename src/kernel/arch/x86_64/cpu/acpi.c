#include <acpi.h>
#include <bitmap.h>
#include <cpu.h>
#include <framebuffer.h>
#include <kheap.h>
#include <logging.h>
#include <numbers.h>
#include <rsdt.h>
#include <stdio.h>
#include <stddef.h>
#include <string.h>
#include <video.h>
#include <vm.h>
#include <vmm.h>
#include <vmm_mapping.h>

RSDT* rsdt_root = NULL;
XSDT* xsdt_root = NULL;

unsigned int rsdtTablesTotal = 0;
uint8_t sdt_version = RSDT_V2;

void parse_SDT(uint64_t address, uint8_t type) {
    if ( type == RSDT_V1) {
        parse_RSDT((RSDPDescriptor *) address);
    } else if ( type == RSDT_V2 ) {
        parse_RSDTv2((RSDPDescriptor20 *) address);
    }
}

void parse_RSDT(RSDPDescriptor *descriptor){
    loglinef(Verbose, "(parse_RSDT): - Parse RSDP Descriptor");
    loglinef(Verbose, "(parse_RSDT): - descriptor Address: 0x%x", descriptor->RsdtAddress);
    map_phys_to_virt_addr((void *) ALIGN_PHYSADDRESS(descriptor->RsdtAddress), (void *) ensure_address_in_higher_half(descriptor->RsdtAddress), VMM_FLAGS_PRESENT | VMM_FLAGS_WRITE_ENABLE);
    _bitmap_set_bit_from_address(ALIGN_PHYSADDRESS(descriptor->RsdtAddress));
    rsdt_root = (RSDT *) ensure_address_in_higher_half((uint64_t) descriptor->RsdtAddress);
    ACPISDTHeader header = rsdt_root->header;
    loglinef(Verbose, "(parse_RSDT): - RSDT_Signature: %.4s", header.Signature);
    loglinef(Verbose, "(parse_RSDT): - RSDT_Lenght: %d", header.Length);
    sdt_version = RSDT_V1;

    // Ok here we are,  and we have mapped the "head of rsdt", it will stay most likely in one page, but there is no way
    // to know the length of the whole table before mapping its header. So now we are able to check if we need to map extra pages
    size_t required_extra_pages = (header.Length / KERNEL_PAGE_SIZE) + 1;
    if (required_extra_pages > 1) {
        //loglinef(Verbose, "(parse_RSDT): - RSDT_PAGES_NEEDED: %d", required_extra_pages);
        for (size_t j = 1; j < required_extra_pages; j++) {
            uint64_t new_physical_address = descriptor->RsdtAddress + (j * KERNEL_PAGE_SIZE);
            map_phys_to_virt_addr((void *) ALIGN_PHYSADDRESS(new_physical_address), (void *) ensure_address_in_higher_half(new_physical_address), VMM_FLAGS_PRESENT | VMM_FLAGS_WRITE_ENABLE);
            _bitmap_set_bit_from_address(ALIGN_PHYSADDRESS(new_physical_address));
        }
    }
    rsdtTablesTotal = (header.Length - sizeof(ACPISDTHeader)) / sizeof(uint32_t);
    loglinef(Verbose, "(parse_RSDT): - Total rsdt Tables: %d", rsdtTablesTotal);

    for(uint32_t i=0; i < rsdtTablesTotal; i++) {
        map_phys_to_virt_addr((void *) ALIGN_PHYSADDRESS(rsdt_root->tables[i]), (void *) ensure_address_in_higher_half(rsdt_root->tables[i]), VMM_FLAGS_PRESENT | VMM_FLAGS_WRITE_ENABLE);
        ACPISDTHeader *tableHeader = (ACPISDTHeader *) ensure_address_in_higher_half(rsdt_root->tables[i]);
        loglinef(Verbose, "(parse_RSDT): \tTable header %d: Signature: %.4s", i, tableHeader->Signature);
    }
}

void parse_RSDTv2(RSDPDescriptor20 *descriptor){
    loglinef(Verbose, "(parse_RSDTv2): Parse RSDP v2 Descriptor\n");
    loglinef(Verbose, "(parse_RSDTv2): - Descriptor physical address: 0x%x", ALIGN_PHYSADDRESS(descriptor->XsdtAddress));
    map_phys_to_virt_addr((void *) ALIGN_PHYSADDRESS(descriptor->XsdtAddress), (void *) ensure_address_in_higher_half(descriptor->XsdtAddress), VMM_FLAGS_PRESENT | VMM_FLAGS_WRITE_ENABLE);
    _bitmap_set_bit_from_address(ALIGN_PHYSADDRESS(descriptor->XsdtAddress));
    xsdt_root = (XSDT *) ensure_address_in_higher_half((uint64_t) descriptor->XsdtAddress);
    loglinef(Verbose, "(parse_RSDTv2): - XSDT_Length: 0x%x", descriptor->Length);
    ACPISDTHeader header = xsdt_root->header;
    loglinef(Verbose, "(parse_RSDTv2): - XSDT_Signature: %.4s", header.Signature);
    sdt_version = RSDT_V2;

    size_t required_extra_pages = (header.Length / KERNEL_PAGE_SIZE) + 1;

    if (required_extra_pages > 1) {
        for (size_t j = 1; j < required_extra_pages; j++) {
            uint64_t new_physical_address = descriptor->XsdtAddress + (j * KERNEL_PAGE_SIZE);
            map_phys_to_virt_addr((uint64_t *) new_physical_address, (uint64_t *) ensure_address_in_higher_half(new_physical_address), VMM_FLAGS_PRESENT | VMM_FLAGS_WRITE_ENABLE);
            _bitmap_set_bit_from_address(ALIGN_PHYSADDRESS(new_physical_address));
        }
    }

    rsdtTablesTotal = (header.Length - sizeof(ACPISDTHeader)) / sizeof(uint64_t);
    loglinef(Verbose, "(parse_RSDTv2): - Total xsdt Tables: %d", rsdtTablesTotal);

    for(uint32_t i=0; i < rsdtTablesTotal; i++) {
        map_phys_to_virt_addr((uint64_t *) ALIGN_PHYSADDRESS(xsdt_root->tables[i]), (uint64_t *) ensure_address_in_higher_half(xsdt_root->tables[i]), VMM_FLAGS_PRESENT | VMM_FLAGS_WRITE_ENABLE);
        _bitmap_set_bit_from_address(ALIGN_PHYSADDRESS(xsdt_root->tables[i]));
        ACPISDTHeader *tableHeader = (ACPISDTHeader *) ensure_address_in_higher_half(xsdt_root->tables[i]);
        loglinef(Verbose, "(parse_RSDTv2): \tTable header %d: Signature: %.4s", i, tableHeader->Signature);
    }

}


ACPISDTHeader* get_SDT_item(char* table_name) {
    if((sdt_version == RSDT_V1 && rsdt_root == NULL) || (sdt_version == RSDT_V2 && xsdt_root == NULL)) {
        return NULL;
    }
    for(uint32_t i=0; i < rsdtTablesTotal; i++){
        ACPISDTHeader *tableItem;
        switch(sdt_version) {
            case RSDT_V1:
                tableItem = (ACPISDTHeader *) ensure_address_in_higher_half(rsdt_root->tables[i]);
                break;
            case RSDT_V2:
                tableItem = (ACPISDTHeader *) ensure_address_in_higher_half(xsdt_root->tables[i]);
                break;
            default:
                logline(Fatal, "(get_SDT_item): )That should not happen, PANIC");
                return NULL;
        }
        int return_value = strncmp(table_name, tableItem->Signature, 4);
        loglinef(Info, "(get_SDT_item): %d - Table name: %.4s", i, tableItem->Signature);
        if(return_value == 0) {
            return tableItem;
        }
    }
    return NULL;
}

/*! \brief It validate the RSDP (v1 and v2) checksum
 *
 * Given the descriptor as a byte array, it sums each byte, and if the last byte of the sum is 0 this means that the structure is valid.
 * @param decriptor the RSDPv1/v2 descriptor
 * @param the size of the struct used by descriptor (should be the size of: RSDPDescriptor or RSDPDescriptorv2)
 * @return true if the validation is succesfull
 */
bool validate_SDT(char *descriptor, size_t size){
    uint32_t sum = 0;
    for (uint32_t i=0; i < size; i++){
        sum += ((char*) descriptor)[i];
    }
    loglinef(Verbose, "(validate_SDT): Checksum of RSDP is: 0x%x", sum&0xFF);
    return ((sum&0xFF) == 0);
}
