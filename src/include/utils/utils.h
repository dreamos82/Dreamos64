#ifndef __UTILS_H__
#define __UTILS_H__

#include <multiboot.h>

#define ELF_MAGIC_SIZE 4
#define USTAR_ID    "ustar"
#define USTAR_SIZE  5
#define USTAR_START_BYTE   257

bool _is_module_elf_hh(struct multiboot_tag_module *loaded_module);
bool _is_module_tar_hh(struct multiboot_tag_module *loaded_module);
bool _is_address_in_multiboot(uint64_t address);

#endif
