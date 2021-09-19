#include <vmm.h>
#include <vm.h>
#include <video.h>
#include <pmm.h>

extern uint64_t p4_table[];
extern uint64_t p3_table_hh[];
extern uint64_t p2_table[];
extern uint64_t pt_tables[];

void _initialize_vmm(){
    _printStr("Coming soon\n");
}

void* request_virtual_page(size_t size){
    return NULL;
}


void free_virtual_page(void *address){

}


void *map_vaddress(void *address, size_t size, int flags){
    uint64_t entry_value;
    uint16_t pml4_e = PML4_ENTRY((uint64_t) address);
    uint16_t pt_e;
    _printStringAndNumber("Mapping address: ", address);
    _printStringAndNumber("PML4 Value: ", pml4_e);
	uint64_t *pml4_table = SIGN_EXTENSION | ENTRIES_TO_ADDRESS(510l,510l,510l,510l);
	if(pml4_table[pml4_e] == 0){
		_printStr("PML4 Entry is 0, need to create a new one");
		uint64_t *new_table = pmm_alloc_frame();
		clean_new_table(new_table);
	}
	_printStringAndNumber("P4_table[pt_e]: ", p4_table[pml4_e]);
    _printStringAndNumber("PML4 Value: ", pml4_table[pml4_e]);
    _printStringAndNumber("P4_table[511]: ", pml4_table[511]);
    _printStringAndNumber("PML4 Value: ", pml4_table[511]);
    uint16_t pdpr_e = PDPR_ENTRY((uint64_t) address);
	uint64_t *pdpr_table = SIGN_EXTENSION | ENTRIES_TO_ADDRESS(510l,510l,510l,pml4_e);
    _printStringAndNumber("PDPR Value: ", pdpr_e);
    _printStringAndNumber("PML4 Value: ", pdpr_table[0]);
    _printStringAndNumber("P4_table[511]: ", pdpr_table[pdpr_e]);
    _printStringAndNumber("PML4 Value: ", pdpr_table[pdpr_e]);
	if(pdpr_table[pdpr_e] == 0){
		_printStr("PDPR - Entry is 0, need to create a new one");
	}

    uint16_t pd_e = PD_ENTRY((uint64_t) address);
    _printStringAndNumber("p3_table_hh[510]: ", p2_table[0]);
    //uint64_t *entry_address = (uint64_t *) (SIGN_EXTENSION |  ENTRIES_TO_ADDRESS(510l, 510l, pml4, pdpr));
    uint64_t *computed_address = SIGN_EXTENSION | ENTRIES_TO_ADDRESS(510l, 510l, 511l, 510l);
    _printStringAndNumber("Computed_address: ", computed_address[0]);
    #if SMALL_PAGES == 1
    pt_e = PT_ENTRY(address);
    _printStringAndNumber("PT_Entry value: ", pt_e);
    #elif SMALL_PAGE == 0 
    //Later
    #endif
}


