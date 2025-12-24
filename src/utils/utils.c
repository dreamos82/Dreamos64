#include <elf.h>
#include <hh_direct_map.h>
#include <logging.h>
#include <main.h>
#include <stdbool.h>
#include <string.h>
#include <utils.h>

bool _is_module_elf_hh (struct multiboot_tag_module *loaded_module) {
    uint64_t module_phys_start = loaded_module->mod_start;
    uint64_t module_phys_end = loaded_module->mod_end;
    uint64_t module_size = module_phys_end - module_phys_start;
    char *module_hh_start =(char *) hhdm_get_variable(module_phys_start);
    pretty_logf(Verbose, "First 3 bytes of module: %c %c %c - ", module_hh_start[1], module_hh_start[2], module_hh_start[3]);
    bool _is_elf = false;
    for (int i = 0; i < ELF_MAGIC_SIZE; i++) {
        if (module_hh_start[i] == _elf_header_mag[i]) {
            _is_elf = true;
        } else {
            _is_elf = false;
            break;
        }
    }
    if (_is_elf == true) {
        pretty_log(Verbose, " The module is an ELF" );
        load_elf((uintptr_t) hhdm_get_variable(module_phys_start), module_size);
        return _is_elf;
    }
    //pretty_logf(Verbose, " loaded_module_address: 0x%x", &loaded_module);
    return _is_elf;
}

bool _is_module_tar_hh(struct multiboot_tag_module *loaded_module) {
    uint64_t module_phys_start = loaded_module->mod_start;
    uint64_t module_phys_end = loaded_module->mod_end;
    uint64_t module_size = module_phys_end - module_phys_start;
    char *module_hh_start =(char *) hhdm_get_variable(module_phys_start);
    bool is_tar = false;
    int result = strncmp(&module_hh_start[USTAR_START_BYTE], USTAR_ID, USTAR_SIZE);
    pretty_logf(Verbose, "ustar id: %c%c%c%c%c - result: %d", module_hh_start[257],module_hh_start[258], module_hh_start[259], module_hh_start[260], module_hh_start[261], result);
    if (result == 0) {
        return true;
    }
    return false;
}


int octascii_to_dec(char *number, int size) {
    int n = 1;
    int converted_value = 0;
    //pretty_logf(Verbose, "Size: %d, number[0]: %d", size, number[0]);
    for (int i=size-2; i >=0; i--) {
        converted_value += (number[i]-0x30) * n;
        //pretty_logf(Verbose, "OctalNumber: %d - cv: %d", number[i]-0x30, converted_value);
        n = 8*n;
    }
    //pretty_logf(Verbose, "Final octalnumber: %d", converted_value);
    return converted_value;
}


#ifndef _TEST_
/**
 * This function checks if given address is within multiboot area.
 *
 * @param address address to check
 * @return 0 if the address is not in multiboot or the first available address;
 */
bool _is_address_in_multiboot(uint64_t address) {
    struct multiboot_tag *tag = tag_start;
    for (tag = tag_start;
        tag->type != MULTIBOOT_TAG_TYPE_END;
        tag = (struct multiboot_tag *) ((multiboot_uint8_t *) tag + ((tag->size + 7) & ~7))){
        if (tag->type == MULTIBOOT_TAG_TYPE_MODULE) {
            struct multiboot_tag_module *loaded_module = (struct multiboot_tag_module *) tag;
            if (address >=  loaded_module->mod_start && address <= loaded_module->mod_end) {
                return true;
            }
        }
    }
    return false;
}
#endif
