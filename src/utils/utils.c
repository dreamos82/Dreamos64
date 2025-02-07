#include <elf.h>
#include <hh_direct_map.h>
#include <logging.h>
#include <main.h>
#include <stdbool.h>
#include <utils.h>

bool load_module_hh (struct multiboot_tag_module *loaded_module) {
    uint64_t module_phys_start = loaded_module->mod_start;
    uint64_t module_phys_end = loaded_module->mod_end;
    uint64_t module_size = module_phys_end - module_phys_start;
    char *module_hh_start =(char *) hhdm_get_variable(module_phys_start);
    pretty_logf(Verbose, "First 3 bytes of module: %c %c %c - ", module_hh_start[1], module_hh_start[2], module_hh_start[3]);
    bool _is_elf = false;
    for (int i = 0; i < 4; i++) {
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

/**
 * T
 *
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
                //pretty_logf(Verbose, "This address: 0x%x is reserved by a multiboot module", address );
                return true;
            }
        }
        //pretty_log(Verbose, " entry not corresponding" );
    }
    return false;
}
