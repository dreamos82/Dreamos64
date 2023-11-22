#include <bitmap.h>
#include <logging.h>
#include <pmm.h>
#include <userspace.h>
#include <vmm.h>
#include <vmm_mapping.h>

unsigned char infinite_loop[] = {
    0xeb, 0xfe
};

uint64_t prepare_userspace_function(VmmInfo *vmm_info) {
    loglinef(Verbose, "(%s): preparing userspace function vmm_data", __FUNCTION__);
    if( vmm_info == NULL ) {
        loglinef(Fatal, "(%s): Error: vmm_info is null");
    }
    void *temp_var = pmm_alloc_frame();
    char *code_page = vmm_alloc(PAGE_SIZE_IN_BYTES, VMM_FLAGS_ADDRESS_ONLY | VMM_FLAGS_WRITE_ENABLE, NULL);
    map_phys_to_virt_addr_hh(temp_var, (void *) code_page, VMM_FLAGS_PRESENT | VMM_FLAGS_WRITE_ENABLE, NULL);
    code_page[0] = infinite_loop[0];
    code_page[1] = infinite_loop[1];
    char *user_code_page = vmm_alloc(PAGE_SIZE_IN_BYTES, VMM_FLAGS_ADDRESS_ONLY | VMM_FLAGS_WRITE_ENABLE | VMM_FLAGS_PRESENT | VMM_FLAGS_USER_LEVEL, vmm_info);
    map_phys_to_virt_addr_hh(temp_var, (void *) user_code_page,VMM_FLAGS_USER_LEVEL  |  VMM_FLAGS_PRESENT | VMM_FLAGS_WRITE_ENABLE, (uint64_t *) vmm_info->root_table_hhdm);
    //loglinef(Verbose, "(%s): user_code_page[0]: 0x%x - user_code_page[1]: 0x%x", __FUNCTION__, user_code_page[0], user_code_page[1]);
    loglinef(Verbose, "(%s): leaving prparing userspace function", __FUNCTION__);
    return (uint64_t) user_code_page;
}
