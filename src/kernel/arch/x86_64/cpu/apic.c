#include <apic.h>
#include <cpu.h>
#include <video.h>

void init_apic(){
    uint32_t apic_supported = _cpuid_feature_apic();
    if (apic_supported == 0x100){
        _printStr("Apic supported\n");
    }
}
