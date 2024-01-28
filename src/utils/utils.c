#include <hh_direct_map.h>
#include <logging.h>
#include <stdbool.h>
#include <utils.h>

bool load_module_hh (struct multiboot_tag_module *loaded_module) {
    uint64_t module_phys_start = loaded_module->mod_start;
    uint64_t module_phys_end = loaded_module->mod_end;
    uint64_t module_size = module_phys_end - module_phys_start;
    char *module_hh_start =(char *) hhdm_get_variable(module_phys_start);
    pretty_logf(Verbose, "First 3 bytes of module: %c %c %c - ", module_hh_start[1], module_hh_start[2], module_hh_start[3]);
    //pretty_logf(Verbose, " loaded_module_address: 0x%x", &loaded_module);
    return false;
}
