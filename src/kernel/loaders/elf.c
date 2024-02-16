#include <elf.h>
#include <logging.h>
#include <utils.h>

const char _elf_header_mag[ELF_MAGIC_SIZE]={0x7f, 'E', 'L', 'F'};

void load_elf(uintptr_t elf_start, uint64_t size) {
    Elf64_Ehdr *elf_header = (Elf64_Ehdr *) elf_start;
    bool is_elf_valid = parse_section_header(elf_header, size, ELF);
    pretty_logf(Verbose, " The elf is valid? %s" , is_elf_valid ? "True" : "False" );
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

        pretty_logf(Verbose, "\tMACHINE 0x%x - 0x%x", elf_start->e_machine);
        if ( elf_start->e_machine != ELF_MACHINE ) {
            return false;
        }

        pretty_logf(Verbose, "\tOS ABI: 0x%x", elf_start->e_ident[EI_OSABI]);
        pretty_logf(Verbose, "\tABI: 0x%x", elf_start->e_ident[EI_ABI]);
        return true;
    }
    return false;
}
