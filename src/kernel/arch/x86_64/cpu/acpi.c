#include <cpu.h>
#include <acpi.h>
#include <video.h>
#include <framebuffer.h>

void parse_RSDT(RSDPDescriptor *descriptor){
    _printStr("Parsing rsdt");
    RSDT_item *root = (RSDT_item *) descriptor->RsdtAddress;
    ACPISDTHeader header = root->header;
    _printStr("SDT_Signature: ");
    _printNewLine();
    //_printStr(header.Signature);
    _printStr("--- ");
    #if USE_FRAMEBUFFER == 1
    _fb_putchar(header.Signature[0], 1, 3, 0x000000, 0xFFFFFF);
    _fb_putchar(header.Signature[1], 2, 3, 0x000000, 0xFFFFFF);
    _fb_putchar(header.Signature[2], 3, 3, 0x000000, 0xFFFFFF);
    _fb_putchar(header.Signature[3], 4, 3, 0x000000, 0xFFFFFF);
    #endif
    _printCh(header.Signature[0], WHITE);
    _printCh(header.Signature[1], WHITE);
    _printCh(header.Signature[2], WHITE);
    _printCh(header.Signature[3], WHITE);
    _printNewLine();
    
}

int validate_RSDP(RSDPDescriptor *descriptor){
    uint8_t sum = 0;
    char number[30];

    for (int i=0; i < sizeof(RSDPDescriptor); i++){
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
