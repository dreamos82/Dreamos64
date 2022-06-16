#include <lapic.h>
#include <cpu.h>
#include <video.h>
#include <msr.h>
#include <bitmap.h>
#include <idt.h>
#include <pic8259.h>
#include <stdio.h>
#include <util.h>
#include <cpuid.h>
#include <io.h>
#include <vmm.h>
#include <kernel.h>
#include <vm.h>
#include <logging.h>

//cpuid is non-standard header, but is supported by both gcc/clang.

extern size_t memory_size_in_bytes;
uint32_t apic_base_address;
uint64_t apic_hh_base_address;
bool apicInX2Mode;

void init_apic() {

    uint64_t msr_output = rdmsr(IA32_APIC_BASE);
    loglinef(Verbose, "APIC MSR Return value: 0x%X", msr_output);
    loglinef(Verbose, "APIC MSR Base Address: 0x%X", (msr_output&APIC_BASE_ADDRESS_MASK));
    apic_base_address = (msr_output&APIC_BASE_ADDRESS_MASK);
    apic_hh_base_address = ensure_address_in_higher_half(apic_base_address);
    if(apic_base_address == 0) {
        logline(Verbose, "ERROR: cannot determine apic base address");
    }

    //determine if x2apic is available, if so, enable it. It's cpuid leaf 1, ecx bit 21.
    uint32_t ignored;
    uint32_t xApicLeaf = 0;
    uint32_t x2ApicLeaf = 0;
    __get_cpuid(1, &ignored, &ignored, &x2ApicLeaf, &xApicLeaf);
    (void)ignored;
    
    if (x2ApicLeaf & (1 << 21)) {
        logline(Verbose, "X2APIC available!");
        apicInX2Mode = true;
        kernel_settings.use_x2_apic = true;
        //no need to map mmio registers as we'll be accessing apic via MSRs
        //we just set bit 10 of the apic base msr, and we're good to go!
        msr_output |= (1 << 10);
        wrmsr(IA32_APIC_BASE, msr_output);
    }
    else if (xApicLeaf & (1 << 9)) {
        logline(Verbose, "APIC available!");
        apicInX2Mode = false;
        kernel_settings.use_x2_apic = false;

        //registers are accessed via mmio, make sure they're identity mapped
        map_phys_to_virt_addr(VPTR(apic_base_address), VPTR(apic_hh_base_address), 0);
    }
    else {
        kernel_settings.use_x2_apic = false;
        logline(Verbose, "ERROR: No local APIC is supported by this cpu!");
        return; //not good.
    }


    uint32_t spurious_interrupt_register = read_apic_register(APIC_SPURIOUS_VECTOR_REGISTER_OFFSET);
    loglinef(Verbose, "Apic enabled: %x - Apic BSP bit: %x", 1&(msr_output >> APIC_GLOBAL_ENABLE_BIT), 1&(msr_output >> APIC_BSP_BIT));
    
    if(!(1&(msr_output >> APIC_GLOBAL_ENABLE_BIT))) {
        logline(Verbose, "Apic disabled globally");
        return;
    }
    
    //Enabling apic
    write_apic_register(APIC_SPURIOUS_VECTOR_REGISTER_OFFSET, APIC_SOFTWARE_ENABLE | APIC_SPURIOUS_INTERRUPT);
    
    if(apic_base_address < memory_size_in_bytes) {
        //I think that ideally it should be relocated above the physical memory (that should be possible)
        //but for now i'll mark that location as used
        logline(Verbose, "Apic base address in physical memory area");
        _bitmap_set_bit(ADDRESS_TO_BITMAP_ENTRY(apic_base_address));
    }
    uint32_t version_register = read_apic_register(APIC_VERSION_REGISTER_OFFSET);
    loglinef(Verbose, "Version register value: 0x%x", version_register);
    loglinef(Verbose, "Spurious vector value: 0x%x", spurious_interrupt_register);
    disable_pic();
}

void disable_pic() {
    outportb(PIC_COMMAND_MASTER, ICW_1);
    outportb(PIC_COMMAND_SLAVE, ICW_1);
    outportb(PIC_DATA_MASTER, ICW_2_M);
    outportb(PIC_DATA_SLAVE, ICW_2_S);
    outportb(PIC_DATA_MASTER, ICW_3_M);
    outportb(PIC_DATA_SLAVE, ICW_3_S);
    outportb(PIC_DATA_MASTER, ICW_4);
    outportb(PIC_DATA_SLAVE, ICW_4);
    outportb(PIC_DATA_MASTER, 0xFF);
    outportb(PIC_DATA_SLAVE, 0xFF);
}

uint32_t read_apic_register(uint32_t register_offset) {
    if (apicInX2Mode)
        return (uint32_t)rdmsr((register_offset >> 4) + 0x800);
    else
        return READMEM32(apic_hh_base_address + register_offset);
}

void write_apic_register(uint32_t register_offset, uint32_t value) {
    if (apicInX2Mode)
        wrmsr((register_offset >> 4) + 0x800, value);
    else
        WRITEMEM32(apic_hh_base_address + register_offset, value);
}

uint32_t lapic_id()
{
    uint32_t id = read_apic_register(APIC_ID_REGISTER_OFFSET);
    return apicInX2Mode ? id : id >> 24;
}

bool lapic_is_x2()
{ 
    return apicInX2Mode; 
}
