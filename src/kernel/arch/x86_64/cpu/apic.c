#include <apic.h>
#include <cpu.h>
#include <video.h>
#include <msr.h>

void init_apic(){
    uint32_t apic_supported = _cpuid_feature_apic();
    if (apic_supported == 0x100){
        _printStr("Apic supported\n");
    }
    uint64_t msr_output = rdmsr(IA32_APIC_BASE);
    printf("APIC MSR Return value: 0x%x\n", msr_output);
    printf("APIC MSR Return value: 0x%x\n", (msr_output&APIC_BASE_ADDRESS_MASK));
}
