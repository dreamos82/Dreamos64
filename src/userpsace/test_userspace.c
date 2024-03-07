#include <bitmap.h>
#include <logging.h>
#include <pmm.h>
#include <userspace.h>
#include <vmm.h>
#include <vmm_mapping.h>

unsigned char infinite_loop[] = {
    0xeb, 0xfe // jmp 0x00
};

unsigned char test_syscall[] = {
    0xbf, 0x63, 0x00, 0x00, 0x00,                   //mov    $0x63,%edi
    0xbe, 0x01, 0x00, 0x00, 0x00,                     //mov    $0x1,%esi
    0xcd, 0x80,                                                 //int    $0x80
    0xeb, 0xf2                                                  //jmp 0x00
};

uint64_t prepare_userspace_function(VmmInfo *vmm_info) {
    pretty_log(Verbose, "Preparing userspace function vmm_data");
    if( vmm_info == NULL ) {
        pretty_log(Fatal, "Error: vmm_info is null");
    }
    void *temp_var = pmm_alloc_frame();
    char *code_page = vmm_alloc(PAGE_SIZE_IN_BYTES, VMM_FLAGS_ADDRESS_ONLY | VMM_FLAGS_WRITE_ENABLE, NULL);
    map_phys_to_virt_addr_hh(temp_var, (void *) code_page, VMM_FLAGS_PRESENT | VMM_FLAGS_WRITE_ENABLE, NULL);
    pretty_logf(Verbose, "Phys address to use: 0x%x, supervisor source address: 0x%x to be used" , temp_var, code_page);
    //code_page[0] = infinite_loop[0];
    //code_page[1] = infinite_loop[1];
    for (int i=0; i < 14; i++) {
        code_page[i] = test_syscall[i];
    }
    char *user_code_page = vmm_alloc(PAGE_SIZE_IN_BYTES, VMM_FLAGS_ADDRESS_ONLY | VMM_FLAGS_WRITE_ENABLE | VMM_FLAGS_PRESENT | VMM_FLAGS_USER_LEVEL, vmm_info);
    map_phys_to_virt_addr_hh(temp_var, (void *) user_code_page,VMM_FLAGS_USER_LEVEL  |  VMM_FLAGS_PRESENT | VMM_FLAGS_WRITE_ENABLE, (uint64_t *) vmm_info->root_table_hhdm);
    pretty_logf(Verbose, "user code page address returned is: 0x%x", user_code_page);
    return (uint64_t) user_code_page;
}
