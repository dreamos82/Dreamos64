#ifndef __UTILS_H__
#define __UTILS_H__

#include <multiboot.h>

bool load_module_hh(struct multiboot_tag_module *loaded_module);
bool _is_address_in_multiboot(uint64_t address);

#endif
