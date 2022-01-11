#include <apic.h>
#include <cpu.h>
#include <video.h>
#include <msr.h>
#include <bitmap.h>
#include <idt.h>
#include <pic8259.h>
#include <stdio.h>

extern uint32_t memory_size_in_bytes;
uint32_t apic_base_address;
uint32_t io_apic_base_address;

void init_apic(){
    uint32_t apic_supported = _cpuid_feature_apic();
    if (apic_supported == 0x100){
        printf("Apic supported\n");
    }
    uint64_t msr_output = rdmsr(IA32_APIC_BASE);
    printf("APIC MSR Return value: 0x%x\n", msr_output);
    printf("APIC MSR Base Address: 0x%x\n", (msr_output&APIC_BASE_ADDRESS_MASK));
    apic_base_address = (msr_output&APIC_BASE_ADDRESS_MASK);
    if(apic_base_address == NULL){
        printf("ERROR: cannot determine apic base address\n");
    }
    map_phys_to_virt_addr(apic_base_address, apic_base_address, 0);
    uint32_t *spurious_interrupt_register = (uint64_t *) (apic_base_address + APIC_SPURIOUS_VECTOR_REGISTER_OFFSET);
    printf("Apic enabled: %x - Apic BSP bit: %x\n", 1&(msr_output >> APIC_GLOBAL_ENABLE_BIT), 1&(msr_output >> APIC_BSP_BIT));
    
    if(!(1&(msr_output >> APIC_GLOBAL_ENABLE_BIT))){
        printf("Apic disabled globally");
        return;
    }
    
    //Enabling apic
    write_apic_register(APIC_SPURIOUS_VECTOR_REGISTER_OFFSET, APIC_SOFTWARE_ENABLE | APIC_SPURIOUS_INTERRUPT);
    
    if(apic_base_address < memory_size_in_bytes){
        //I think that ideally it should be relocated above the physical memory (that should be possible)
        //but for now i'll mark that location as used
        printf("Apic base address in physical memory area");
        _bitmap_set_bit(ADDRESS_TO_BITMAP_ENTRY(apic_base_address));
    }
    uint32_t version_register = *(uint32_t *) (apic_base_address + APIC_VERSION_REGISTER_OFFSET);
    printf("Version register value: 0x%x\n", version_register);
    printf("Spurious vector value: 0x%x\n", *spurious_interrupt_register);
    disable_pic();
}

void disable_pic(){
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

void init_ioapic(MADT *madt_table){
    MADT_Item* item = get_MADT_item(madt_table, MADT_IO_APIC, 0);
    if(item != NULL) {
        printf("Item found: type: %d - Requested: %d\n", item->type, MADT_IO_APIC);
        IO_APIC_Item *ioapic_item = (IO_APIC_Item *) ((uint32_t) item + sizeof(MADT_Item));
        printf("IOAPIC_ID: 0x%x\n", ioapic_item->ioapic_id); 
        printf("IOAPIC_ID address: 0x%x\n", ioapic_item->address);
        printf("IOApic_Global_System_Interrupt_Base: 0x%x\n", ioapic_item->global_system_interrupt_base);
        io_apic_base_address = ioapic_item->address;
        map_phys_to_virt_addr(io_apic_base_address, io_apic_base_address, 0);
        _bitmap_set_bit(ADDRESS_TO_BITMAP_ENTRY(io_apic_base_address));
        uint32_t ioapic_version = read_io_apic_register(IO_APIC_VER_OFFSET);
        printf("IOAPIC Version: 0x%x\n", ioapic_version);
    }
}

void start_apic_timer(uint16_t flags, bool periodic){

    if(apic_base_address == NULL){
        printf("Apic_base_address not found, or apic not initialized\n");
    }

    printf("Read apic_register: 0x%x\n", read_apic_register(APIC_TIMER_LVT_OFFSET));

    uint32_t *apic_timer_configuration_register = (uint32_t *) (apic_timer_configuration_register + APIC_TIMER_CONFIGURATION_OFFSET);
    write_apic_register(APIC_TIMER_INITIAL_COUNT_REGISTER_OFFSET, 0x100000);
    write_apic_register(APIC_TIMER_CONFIGURATION_OFFSET, APIC_TIMER_DIVIDER_1);
    write_apic_register(APIC_TIMER_LVT_OFFSET, APIC_TIMER_IDT_ENTRY);
    
    asm("sti");
}

uint32_t read_apic_register(uint32_t register_offset){
    uint32_t *reg_address = (uint32_t *) (apic_base_address + register_offset);
    return *reg_address;
}

void write_apic_register(uint32_t register_offset, uint32_t value){
    uint32_t *reg_address = (uint32_t *) (apic_base_address + register_offset);
    *reg_address = value;
}

uint32_t read_io_apic_register(uint8_t offset){
    if (io_apic_base_address == NULL) {
        printf("It's null");
        return NULL;
    }
    *(volatile uint32_t*) io_apic_base_address = offset;
    return *(volatile uint32_t*) (io_apic_base_address + 0x10);
}

IOREDTBL_Entry read_io_apic_ioredtbl(uint8_t index){
}

