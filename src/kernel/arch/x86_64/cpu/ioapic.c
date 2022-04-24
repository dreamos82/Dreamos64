#include <ioapic.h>
#include <bitmap.h>
#include <stdio.h>
#include <stddef.h>
#include <vmm.h>
#include <vm.h>

uint32_t io_apic_base_address;
uint8_t io_apic_max_redirections = 0;
IO_APIC_source_override_item_t io_apic_source_overrides[IO_APIC_SOURCE_OVERRIDE_MAX_ITEMS];
uint8_t io_apic_source_override_array_size;

void init_ioapic(MADT *madt_table){
    MADT_Item* item = get_MADT_item(madt_table, MADT_IO_APIC, 0);
    io_apic_source_override_array_size = 0;
    if(item != NULL) {
        printf("IOAPIC Item type: 0x%x\n", item->type);
        printf("IOAPIC Item address: 0x%x - length: 0x%x\n", item, item->length);
        IO_APIC_Item *ioapic_item = (IO_APIC_Item *) ( ensure_address_in_higher_half((uint64_t) item + sizeof(MADT_Item)));
        if (is_phyisical_address_mapped(ALIGN_PHYSADDRESS((uint64_t) item), ensure_address_in_higher_half((uint64_t) item)) == PHYS_ADDRESS_NOT_MAPPED) {
            map_phys_to_virt_addr((uint64_t *) ALIGN_PHYSADDRESS((uint64_t) item), ensure_address_in_higher_half((uint64_t) item),0);
        }
        printf("IOAPIC_ID: 0x%x, Address: 0x%x\n", ioapic_item->ioapic_id, ioapic_item->address ); 
        printf("IOApic_Global_System_Interrupt_Base: 0x%x\n", ioapic_item->global_system_interrupt_base);
        io_apic_base_address = ioapic_item->address;
        // This one should be mapped in the higher half ?? 
        map_phys_to_virt_addr((uint64_t *) io_apic_base_address, (uint64_t *) io_apic_base_address, 0);
        _bitmap_set_bit(ADDRESS_TO_BITMAP_ENTRY(io_apic_base_address));
        uint32_t ioapic_version = read_io_apic_register(IO_APIC_VER_OFFSET);
        printf("IOAPIC Version: 0x%x\n", ioapic_version);
        io_apic_redirect_entry_t redtbl_entry;
        int return_value = read_io_apic_redirect(0x10, &redtbl_entry);
        if (return_value != 0) {
            printf("PANIC!!\n");
        }
        io_apic_max_redirections = (uint8_t) (ioapic_version >> 16);
        printf("Max redirection entries value: 0x%x\n ", io_apic_max_redirections);
        io_apic_source_override_array_size = parse_io_apic_interrupt_source_overrides(madt_table);
        printf("Found %x source override entries\n", io_apic_source_override_array_size);
        printf("---- SO Item: bus_source: %x - irq_source: %x - GSI: %x\n", io_apic_source_overrides[1].bus_source, io_apic_source_overrides[1].irq_source, io_apic_source_overrides[1].global_system_interrupt);
    }
}

int parse_io_apic_interrupt_source_overrides(MADT* table) {
    printf("madt_address: 0x%x\n", (uint64_t) table);
    uint32_t total_length = sizeof(MADT);
    uint32_t source_override_counter = 0;
    //MADT_Item* item = (MADT_Item *) ((uint32_t)table + sizeof(MADT));
    MADT_Item* item = get_MADT_item(table, MADT_IO_APIC_INTERRUPT_SOURCE_OVERRIDE, source_override_counter);
    printf("Table Lenght: %x\n", table->header.Length);
    while(total_length < table->header.Length && source_override_counter < IO_APIC_SOURCE_OVERRIDE_MAX_ITEMS) {
        total_length = total_length + item->length;
        if(item != NULL && item->type == MADT_IO_APIC_INTERRUPT_SOURCE_OVERRIDE){
            IO_APIC_source_override_item_t *so_item = (IO_APIC_source_override_item_t *) (item + 1);
            io_apic_source_overrides[source_override_counter].bus_source = so_item->bus_source;
            io_apic_source_overrides[source_override_counter].irq_source = so_item->irq_source;
            io_apic_source_overrides[source_override_counter].global_system_interrupt = so_item->global_system_interrupt;
            io_apic_source_overrides[source_override_counter].flags = so_item->flags;
            printf("--SO Item: type: %x  bus_src: %x - irq_src: %x - GSI: %x -\n", item->type, so_item->bus_source, so_item->irq_source, so_item->global_system_interrupt);
            source_override_counter++;
        }
        //item = (MADT_Item *)((uint32_t)table + total_length);
        item = get_MADT_item(table, MADT_IO_APIC_INTERRUPT_SOURCE_OVERRIDE, source_override_counter);
        if(item == NULL) break;
    }
    return source_override_counter;
}


uint32_t read_io_apic_register(uint8_t offset){
    if (io_apic_base_address == 0) {
        printf("It's null");
        return 0;
    }
    *(volatile uint32_t*) io_apic_base_address = offset;
    return *(volatile uint32_t*) (io_apic_base_address + 0x10);
}

int read_io_apic_redirect(uint8_t index, io_apic_redirect_entry_t *redtbl_entry){
    if (index < 0x10 && index > 0x3F) {
        return -1;
    }
    if ((index%2) != 0) {
        return -1;
    }
    uint32_t lower_part;
    uint32_t upper_part;
    lower_part = read_io_apic_register(index);
    upper_part = read_io_apic_register(index+1);
    uint64_t raw_value = ((uint64_t) upper_part << 32) | ((uint64_t) lower_part);
    redtbl_entry->raw = raw_value;
    return 0;
}

int write_io_apic_redirect(uint8_t index, io_apic_redirect_entry_t redtbl_entry) {
    if (index < 0x10 && index > 0x3F) {
        return -1;
    }
    if ((index%2) != 0) {
        return -1;
    }
    printf("Setting redirect entry: %x\n", index);
    uint32_t upper_part = (uint32_t) (redtbl_entry.raw >> 32);
    uint32_t lower_part = (uint32_t) redtbl_entry.raw;
    /* printf(": Lower part: %x, : upper part: %x\n", lower_part, upper_part); */
    write_io_apic_register(index, lower_part);
    write_io_apic_register(index+1, upper_part);
    return 0;
}

void write_io_apic_register(uint8_t offset, uint32_t value) {
    *(volatile uint32_t*) io_apic_base_address = offset;
    *(volatile uint32_t*) (io_apic_base_address + 0x10) = value;
}

void set_irq(uint8_t irq_type, uint8_t redirect_table_pos, uint8_t idt_entry, uint8_t destination_field, uint32_t flags, bool masked) {

    // masked: if true the redirection table entry is set, but the IRQ is not enabled.
    // 1. Check if irq_type is in the Source overrides
    uint8_t counter = 0;
    uint8_t selected_pin = irq_type;
    io_apic_redirect_entry_t entry; 
    entry.raw = flags | (idt_entry & 0xFF);
    while(counter < io_apic_source_override_array_size) {
        // 2. If yes we need to use the gsi in the source override entry
        if(io_apic_source_overrides[counter].irq_source == irq_type) {
            selected_pin = io_apic_source_overrides[counter].global_system_interrupt;
            printf("---Source Override found for type: %x using apic pin: %x ", irq_type, selected_pin);
            if((io_apic_source_overrides[counter].flags & 0b11) == 2) {
                entry.pin_polarity = 0b1;
            } else  {
                entry.pin_polarity = 0b0;
            }
            if(((io_apic_source_overrides[counter].flags >>2) & 0b11) == 2) {
                entry.pin_polarity = 0b1;
            } else  {
                entry.pin_polarity = 0b0;
            }
            break;
        }
        counter++;
    }
    // Set the destination api
    entry.destination_field = destination_field;
    entry.interrupt_mask = masked;
    printf("Setting IRQ number: %x, to idt_entry: %x at REDTBL pos: %x - Final value: %x\n", irq_type, idt_entry, redirect_table_pos, entry.raw);
    write_io_apic_redirect(redirect_table_pos, entry);
    io_apic_redirect_entry_t read_entry; 
    int ret_val = read_io_apic_redirect(IOREDTBL1, &read_entry);
    printf("ret_val: %d - entry raw: %x mask: %d\n", ret_val, read_entry.vector, read_entry.interrupt_mask);
}

int set_irq_mask(uint8_t redirect_table_pos, bool masked_status) {
    // Masked status: true = masked false = enabled
    io_apic_redirect_entry_t read_entry;
    int ret_val = read_io_apic_redirect(redirect_table_pos, &read_entry);
    if(ret_val == 0) {
        read_entry.interrupt_mask = masked_status;
        write_io_apic_redirect(redirect_table_pos, read_entry);
    } else  {
        printf("Invalid redirection table position\n");
        return -1;
    }
    return 0;
}

