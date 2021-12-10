#include <apic.h>
#include <cpu.h>
#include <video.h>
#include <msr.h>
#include <bitmap.h>

extern uint32_t memory_size_in_bytes;
uint32_t apic_base_address;

void init_apic(){
    uint32_t apic_supported = _cpuid_feature_apic();
    if (apic_supported == 0x100){
        _printStr("Apic supported\n");
    }
    uint64_t msr_output = rdmsr(IA32_APIC_BASE);
    printf("APIC MSR Return value: 0x%x\n", msr_output);
    printf("APIC MSR Base Address: 0x%x\n", (msr_output&APIC_BASE_ADDRESS_MASK));
    apic_base_address = (msr_output&APIC_BASE_ADDRESS_MASK);
    printf("Apic enabled: %x\n", 1&(msr_output >> APIC_GLOBAL_ENABLE_BIT));
    printf("Apic BSP: %x\n", 1&(msr_output >> APIC_BSP_BIT));
    
    if(!(1&(msr_output >> APIC_GLOBAL_ENABLE_BIT))){
        printf("Apic not enabled!");
        return;
    }

    printf("Check memory_size_in bytes: 0x%x\n", memory_size_in_bytes);
    if(apic_base_address < memory_size_in_bytes){
        //I think that ideally it should be relocated above the physical memory (that should be possible)
        //but for now i'll mark that location as used
        printf("Apic base address in physical memory area");
        _bitmap_set_bit(ADDRESS_TO_BITMAP_ENTRY(apic_base_address));
    }
    map_vaddress(apic_base_address, 0);
    uint32_t timer_value = (*(uint32_t*) (apic_base_address + APIC_TIMER_OFFSET));
    printf("Timer value: 0x%x\n", timer_value);
}
