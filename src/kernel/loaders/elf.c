#include <bitmap.h>
#include <elf.h>
#include <logging.h>
#include <utils.h>
#include <vmm_util.h>

const char _elf_header_mag[ELF_MAGIC_SIZE]={0x7f, 'E', 'L', 'F'};

void load_elf(uintptr_t elf_start, uint64_t size) {
    Elf64_Ehdr *elf_header = (Elf64_Ehdr *) elf_start;
    bool is_elf_valid = parse_section_header(elf_header, size, ELF);
    pretty_logf(Verbose, " The elf is valid? %s" , is_elf_valid ? "True" : "False" );
    if (is_elf_valid) {
        pretty_log(Verbose, " Going to load headers");
        Elf64_Half phdr_entries = elf_header->e_phnum;
        Elf64_Half phdr_entsize = elf_header->e_phentsize;
        pretty_logf(Verbose, " Number of PHDR entries: 0x%x", phdr_entries);
        pretty_logf(Verbose, " PHDR Entry Size: 0x%x", phdr_entsize );
        pretty_logf(Verbose, " ELF Entry point: 0x%x", elf_header->e_entry);
        Elf64_Half result = loop_phdrs(elf_header, phdr_entries);
        if (result > 0) {
            pretty_logf(Verbose, " Number of PT_LOAD entries: %d", result);
            Elf64_Phdr *cur_phdr = read_phdr(elf_header, 0);
            pretty_logf(Verbose, "\t[cur_phdr]: Type: 0x%x, Flags: 0x%x  -  Vaddr: 0x%x - aligned: 0x%x  - p_align: 0x%x - p_memsz: 0%x - p_offset: 0x%x", cur_phdr->p_type, cur_phdr->p_flags, cur_phdr->p_vaddr, align_value_to_page(cur_phdr->p_vaddr), cur_phdr->p_align, cur_phdr->p_memsz, cur_phdr->p_offset);
            cur_phdr = read_phdr(elf_header, 1);
        }
    }
}

Elf64_Half loop_phdrs(Elf64_Ehdr* e_hdr, Elf64_Half phdr_entries) {
    Elf64_Phdr *phdr_list = (Elf64_Phdr*) ((uintptr_t) e_hdr + e_hdr->e_phoff);
    Elf64_Half number_of_pt_loads = 0;
    for (size_t i = 0; i < phdr_entries; i++) {
        Elf64_Phdr phdr = phdr_list[i];
        pretty_logf(Verbose, "\t[%d]: Type: 0x%x, Flags: 0x%x  -  Vaddr: 0x%x - aligned: 0x%x - offset: 0x%x ", i, phdr.p_type, phdr.p_flags, phdr.p_vaddr, align_value_to_page(phdr.p_vaddr), phdr.p_offset);
        if ( is_address_aligned(phdr.p_vaddr, PAGE_SIZE_IN_BYTES) ) {
            pretty_log(Verbose, "\tThe address is aligned");
        } else {
            pretty_log(Verbose, "\tThe address is not aligned");
        }
        if ( phdr.p_type == PT_LOAD ) number_of_pt_loads++;
    }
    return number_of_pt_loads;
}

/**
 * This function return the phdr entry at the pdhrd_entry_number provided.
 *
 *
 * @param e_hdr the elf header
 * @param phdr_entry_number the entry number we want to read
 * @return  Elf64_Phdr * the selected P_hdr or NULL if not found.
 */
Elf64_Phdr *read_phdr(Elf64_Ehdr* e_hdr, Elf64_Half phdr_entry_number) {
    Elf64_Half phdr_entries = e_hdr->e_phnum;

    if ( phdr_entry_number <  phdr_entries) {
        Elf64_Phdr *phdr_list = (Elf64_Phdr*) ((uintptr_t) e_hdr + e_hdr->e_phoff);
        return &phdr_list[phdr_entry_number];
    }

    return NULL;
}

bool validate_elf_magic_number(Elf64_Ehdr *elf_start){
    for (int i = 0; i < ELF_MAGIC_SIZE; i++) {
            if (!(elf_start->e_ident[i] == _elf_header_mag[i])) {
                return false;
            }
    }
    return true;
}



bool parse_section_header(Elf64_Ehdr *elf_start, uint64_t size, executable_loader_type type) {
    bool value_to_return = false;
    if ( type == ELF ) {
        for (int i = 0; i < ELF_MAGIC_SIZE; i++) {
            if (elf_start->e_ident[i] == _elf_header_mag[i]) {
                pretty_logf(Verbose, " i: %d: elf_start = %c : _elf_header_mag = %c", i, elf_start->e_ident[i], _elf_header_mag[i]);
            }
        }
        pretty_log(Verbose, "Nident flags:" );

        pretty_logf(Verbose, "\tCLASS: 0x%x", elf_start->e_ident[EI_CLASS]);
        if ( elf_start->e_ident[EI_CLASS] != ELFCLASS64 ) {
            return false;
        }

        pretty_logf(Verbose, "\tDATA: 0x%x", elf_start->e_ident[EI_DATA]);
        if ( elf_start->e_ident[EI_DATA] != ELFDATA2LSB ) {
            return false;
        }

        pretty_logf(Verbose, "\tVERSION: 0x%x", elf_start->e_ident[EI_VERSION]);
        if ( elf_start->e_ident[EI_VERSION] != EV_CURRENT ) {
            return false;
        }

        pretty_logf(Verbose, "\tMACHINE: 0x%x", elf_start->e_machine);
        if ( elf_start->e_machine != ELF_MACHINE ) {
            return false;
        }

        pretty_logf(Verbose, "\tTYPE: 0x%x", elf_start->e_type);

        pretty_logf(Verbose, "\tOS ABI: 0x%x", elf_start->e_ident[EI_OSABI]);
        pretty_logf(Verbose, "\tABI: 0x%x", elf_start->e_ident[EI_ABI]);
        return true;
    }
    return false;
}

/**
 * This function given an elf p_hdr flag  returns the architecture dependent vmm flags
 *
 *
 * @param flags elf flags
 * @return architecture dependant flags
 */
uint64_t elf_flags_to_memory_flags(Elf64_Word flags) {
    // This function will be movede into the arch dependant code
    // Conversion
    // ELF   |   VMM
    // 0x0   |   Executable
    // 0x1   |   Write
    // 0x2   |   Read
    uint64_t flags_to_return = (flags & 0b10);
    return flags_to_return;
}
