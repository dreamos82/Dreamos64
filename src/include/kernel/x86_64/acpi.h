#ifndef _ACPI_H
#define _ACPI_H

#include <stdint.h>
#include <cpu.h>

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
} ACPISDTHeader;

typedef struct {
    ACPISDTHeader header;
    uint32_t nextSDT_item[1];
} RSDT_item;

void parse_RSDT(RSDPDescriptor *);
void parse_RSDTv2(RSDPDescriptor20 *);
int validate_RSDP(RSDPDescriptor *);
#endif
