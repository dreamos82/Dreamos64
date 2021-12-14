#include <vmm.h>
#include <vm.h>
#include <video.h>
#include <pmm.h>

extern uint64_t p4_table[];
extern uint64_t p3_table_hh[];
extern uint64_t p2_table[];
extern uint64_t pt_tables[];

int unmap_vaddress(void *address){
	uint16_t pml4_e = PML4_ENTRY((uint64_t) address);
	uint64_t *pml4_table = (uint64_t *) (SIGN_EXTENSION | ENTRIES_TO_ADDRESS(510l,510l,510l,510l));
	if(!(pml4_table[pml4_e] &0b1)){
		return -1;
	}
    uint16_t pdpr_e = PDPR_ENTRY((uint64_t) address);
	uint64_t *pdpr_table = (uint64_t *) (SIGN_EXTENSION | ENTRIES_TO_ADDRESS(510l,510l,510l,pml4_e));
	if(!(pdpr_table[pdpr_e] & 0b1)){
		return -1;
	}
	uint64_t *pd_table = (uint64_t *) (SIGN_EXTENSION | ENTRIES_TO_ADDRESS(510l,510l,pml4_e, pdpr_e));
    uint16_t pd_e = PD_ENTRY((uint64_t) address);
	if(!(pd_table[pd_e] & 0b01)){
		return -1;
	}
	
	#if SMALL_PAGES == 0
	_printStr("Freeing page\n");
	pd_table[pd_e] = 0x0l;
	invalidate_page_table(pd_table);
	#elif SMALL_PAGES == 1
	uint64_t *pt_table = SIGN_EXTENSION | ENTRIES_TO_ADDRESS(510l, pml4_e, pdpr_e, pd_e);
    uint16_t pt_e = PT_ENTRY((uint64_t) address);

	if(!(pt_table[pt_e] & 0b1)) {
		return -1;
	} 
	pt_table[pt_e] = 0x0l;
	invalidate_page_table(address);
	#endif

	return 0;
}


void *map_phys_to_virt_addr(void* physical_address, void* address, unsigned int flags){
    uint16_t pml4_e = PML4_ENTRY((uint64_t) address);
    _printStringAndNumber("Mapping address: ", address);
    _printStringAndNumber("PML4 Value: ", pml4_e);
	uint64_t *pml4_table = (uint64_t *) (SIGN_EXTENSION | ENTRIES_TO_ADDRESS(510l,510l,510l,510l));
	if(!(pml4_table[pml4_e] & 0b1)){
		uint64_t *new_table = pmm_alloc_frame();
		pml4_table[pml4_e] = (uint64_t) new_table | WRITE_BIT | PRESENT_BIT;
		clean_new_table(new_table);
	}
#if VERBOSE_OUTPUT == 1
	_printStringAndNumber("P4_table[pml4_e]: ", p4_table[pml4_e]);
    _printStringAndNumber("PML4 Value: ", pml4_table[pml4_e]);
#endif
    uint16_t pdpr_e = PDPR_ENTRY((uint64_t) address);
	uint64_t *pdpr_table = (uint64_t *) (SIGN_EXTENSION | ENTRIES_TO_ADDRESS(510l,510l,510l,pml4_e));
#if VERBOSE_OUTPUT == 1
    _printStringAndNumber("PDPR Value: ", pdpr_e);
    _printStringAndNumber("PDPR Table[pdpr_e]: ", pdpr_table[pdpr_e]);
#endif
	if(!(pdpr_table[pdpr_e] & 0b1)){
		uint64_t *new_table = pmm_alloc_frame();
		pdpr_table[pdpr_e] = (uint64_t) new_table | WRITE_BIT | PRESENT_BIT;
		clean_new_table(new_table);
	}
	uint64_t *pd_table = (uint64_t *) (SIGN_EXTENSION | ENTRIES_TO_ADDRESS(510l,510l,pml4_e, pdpr_e));
    uint16_t pd_e = PD_ENTRY((uint64_t) address);
	if(!(pd_table[pd_e] & 0b01)){
		uint64_t *new_table = pmm_alloc_frame();
		#if SMALL_PAGES == 1
		pd_table[pd_e] = (uint64_t) new_table | flags | WRITE_BIT | PRESENT_BIT;
		#elif SMALL_PAGES == 0
		pd_table[pd_e] = (uint64_t) physical_address | flags | WRITE_BIT | PRESENT_BIT | HUGEPAGE_BIT;
		clean_new_table(new_table);
		#endif
	}

    #if SMALL_PAGES == 1
	uint64_t *pt_table = (uint64_t *) (SIGN_EXTENSION | ENTRIES_TO_ADDRESS(510l, pml4_e, pdpr_e, pd_e));
    uint16_t pt_e = PT_ENTRY((uint64_t) address);
	if(!(pt_table[pt_e] & 0b1)) {
		uint64_t *new_table = pmm_alloc_frame();
		pt_table[pt_e] = (uint64_t) physical_address | WRITE_BIT | PRESENT_BIT;
	}
	#endif


	return address;
}

void *map_vaddress(void *virtual_address, unsigned int flags){
    void *new_addr = pmm_alloc_frame();
    map_phys_to_virt_addr(new_addr, virtual_address, flags);
}


