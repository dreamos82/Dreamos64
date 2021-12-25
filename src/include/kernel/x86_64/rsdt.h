#ifndef _RSDT_H_
#define _RSDT_H_

#include <acpi.h> 
#include <stdint.h>

typedef struct RSDT {
  ACPISDTHeader header;
  uint32_t tables[];
} RSDT;


extern uint32_t rsdtTablesTotal;
extern RSDT* rsdt_root;
#endif
