#ifndef _ACPI_H
#define _ACPI_H

#include <stdint.h>
#include <cpu.h>

#define RSDT_V1 14
#define RSDT_V2 15

typedef struct {
  char Signature[4];
  uint32_t Length;
  uint8_t Revision;
  uint8_t Checksum;
  char OEMID[6];
  char OEMTableID[8];
  uint32_t OEMRevision;
  uint32_t CreatorID;
  uint32_t CreatorRevision;
} __attribute__((packed)) ACPISDTHeader;

typedef struct {
    ACPISDTHeader header;
    uint32_t nextSDT_item[1];
} __attribute__((packed)) RSDT_item;

void parse_SDT(uint64_t, uint8_t);
void parse_RSDT(RSDPDescriptor *);
void parse_RSDTv2(RSDPDescriptor20 *);
int validate_RSDP(RSDPDescriptor *);

ACPISDTHeader* get_RSDT_Item(char *);
#endif
